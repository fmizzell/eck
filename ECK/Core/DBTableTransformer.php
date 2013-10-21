<?php
namespace ECK\Core;
use Exception;
/**
 * This class simplifies making modifications to a db table
 */
class DBTableTransformer{
  
  private $table;
  private $add_fields;
  private $remove_fields;
  private $delete_table;
  private $create_table;
  private $schema;
  
  public function __construct($table, $schema){
    $this->schema = $schema;
    $this->table = $table;
    $this->add_fields = array();
    $this->remove_fields = array();
    $this->delete_table = FALSE;
    $this->create_table = FALSE;
  }
  
  public function createTable(){
    $this->create_table = TRUE;
  }
  
  public function deleteTable(){
    $this->delete_table = TRUE;
  }
  
  public function addField($name, $schema){
    $this->add_fields[$name] = $schema;
  }
  
  public function removeField($name){
    $this->remove_fields[$name] = TRUE;
  }
  
  public function performTransformations(){
    //Does this table exist
    $exists = db_table_exists($this->table);
    
    //Delete table
    if($exists && $this->delete_table){
      db_drop_table($this->table);
      //and we are done
      return TRUE;
    }else if(!$exists && $this->delete_table){
      throw new Exception("The db table {$this->table} does not exist, so it can not
      be deleted");
    }
    
    //Create table
    if(!$exists && $this->create_table){
      $schema = $this->getSchema();
      db_create_table($this->table, $schema);
      $exists = TRUE;
    }else if($exists && $this->create_table){
      throw new Exception("The db table {$this->table} already exist, so it can not
      be created");
    }
    
    //Add fields
    if($exists && !empty($this->add_fields)){
      foreach($this->add_fields as $name => $schema){
        db_add_field($this->table, $name, $schema);
      }
    }else if(!$exists && !empty($this->add_fields)){
      throw new Exception("Can't add fields to the non existant table {$this->table}");
    }
    
    //Delete fields
    if($exists && !empty($this->remove_fields)){
      foreach($this->remove_fields as $name => $bool){
        db_drop_field($this->table, $name);
      }
    }else if(!$exists && !empty($this->remove_fields)){
      throw new Exception("Can't delet fields from the non existant table {$this->table}");
    }
  }
  
  private function getSchema(){

    // Add properties to schema definition.
    $this->schema['fields'] += $this->add_fields;

    return $this->schema;
  }
}
  

