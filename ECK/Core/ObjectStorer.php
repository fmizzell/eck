<?php
namespace ECK\Core;
use ReflectionClass;

define("OBJECT_STORER_CREATED", 1);
define("OBJECT_STORER_UPDATED", 2);

/**
 * As the name suggests, this class is responsible for storing objects
 */

class ObjectStorer{
  //the class of the objects we are saving or loading
  private $class;
  //constructor argument (the fields from the table that should be passed as arguments
  private $constructor_args;
  //The database table where the objects exist
  private $table;
  //the primary keys of the table
  private $primary_keys;
  //the properties that can be stored in the table
  private $fields;
  //which properties are stored serialized
  private $serialize;

  public function __construct($class, $constructor_args, $table){
    $this->class = $class;
    $this->constructor_args = $constructor_args;
    $this->serialize = array();
    
    //is this a real table, check it
    if($schema = drupal_get_schema($table)){
      $this->table = $table;
      $this->primary_keys = $schema["primary key"];
      $this->fields = array_keys($schema['fields']);

      //do we want to handle searialized variables by default? let's do it
      //and wait for some critizism
      foreach($schema['fields'] as $name => $field){
        if(array_key_exists('serialize', $field) && $field['serialize']){
          $this->serialize[] = $name;
        }
      }
    }else{
      throw new Exception("The table '{$table}' does not exist");
    }
  }

  //DB Interaction Functions
  public function save($object){
    //first lets get all the fields that need to be saved from the object
    $data = array();
    
    foreach($this->fields as $field){
      $value = NULL;
      if(in_array($field, $this->serialize)){
        $value = $this->getValue($object, $field, TRUE);
      }else{
        $value = $this->getValue($object, $field, FALSE);
      }
      if($value){
        $data[$field] = $value;
      }
    }
    
    $is_new = FALSE;
    //now lets check whether this object is new 
    $primary_key = $this->primary_keys[0];
    $have_primary = array_key_exists($primary_key, $data);
    if(!$have_primary){
      $is_new = TRUE;
    }else{
      $query = db_select($this->table, 't');
      $query->fields('t', array($primary_key));
      $query->condition($primary_key, $data[$primary_key]);
      $results = $query->execute()->fetchAll();
      if(empty($results)){
        $is_new = TRUE;
      }
    }

    if($is_new){
      db_insert($this->table)
      ->fields($data)
      ->execute();
      return OBJECT_STORER_CREATED;
    }else{
      //well I need to know what the primary id is to set up the condition;
      db_update($this->table)
      ->condition($primary_key, $data[$primary_key], '=')
      ->fields($data)
      ->execute();
      return OBJECT_STORER_UPDATED;
    }
  }

  public function load($property = NULL, $value = NULL){
    $query = db_select($this->table, 't');
    $query->fields('t');
    
    if($property && $value){
      $query->condition($property,  $value, '=');
    }
    
    $executed = $query->execute();
    $results = $executed->fetchAll();
    
    $objects = array();

    foreach($results as $result){
      $objects[] = $this->buildObject((array)$result);
    }
    
    return $objects;
  }
  
  /**
   * Take an array with the fields from the db and constructs an object
   * @param type $properties 
   */
  private function buildObject($properties){
    
    $constructor_argument_values = array();
    foreach($this->constructor_args as $arg){
      $constructor_argument_values[$arg] = $properties[$arg];
      unset($properties[$arg]);
    }
    
    $reflector = new ReflectionClass($this->class);
    $object = $reflector->newInstanceArgs($constructor_argument_values);
    foreach($properties as $property => $value){
      if(in_array($property, $this->serialize)){
        $this->setValue($object, $property, $value, TRUE);
      }else{
        $this->setValue($object, $property, $value);
      }
    }
    
    return $object;
}

  private function setValue($object, $field, $value, $serial = FALSE){
    if($serial){
      $method_name = "deserialize".ucfirst($field);
    }else{
      $method_name = "set".ucfirst($field);
    }
    if(method_exists($object, $method_name)){
      $object->{$method_name}($value);
    }else{
      try{
        $object->{$field} = $value;
      }catch(Exception $e){
        throw new Exception("Thre isn't a setter for {$field} and it seems
        to be a private property");
      }
    }
  }
   
  public function delete($object){
    // we need to get the primary key
    $pk = $this->primary_keys[0];
    
    $pk_value = $this->getValue($object, $pk);
    
    //if we have a primary key value, we can delete the object
    //most of the time
    if($pk_value){
      $query = db_delete($this->table);
      $query->condition($pk, $pk_value, '=');
      $query->execute();
    }
  }
  
  private function getValue($object, $field, $serial = FALSE){
    $method_name = "get".ucfirst($field);
    
    if($serial){
      $method_name = "serialize".ucfirst($field);
    }else{
      $method_name = "get".ucfirst($field);
    }
    
    if(method_exists($object, $method_name)){
      return $object->{$method_name}();
    }else if(property_exists($object, $field)){
      try{
        $value = $object->{$field};
      }catch(Exception $e){
        return NULL;
      }
      return $value;
    }
    
    return NULL;
  }
}

