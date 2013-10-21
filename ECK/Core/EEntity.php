<?php
namespace ECK\Core;
use Entity;

class EEntity extends Entity{

  //this flag only gets set so we can make our properties public again
  //before we save the object;
  protected $ignore_validation;
  protected $entity_type;
  protected $property_names;
  protected $property_values = array();

  public function __construct(array $values = array(), $entity_type = NULL){

    $this->ignore_validation = FALSE;
    
    //I have this stupid crap.. fixing drupals mess ups.
    $this->entity_type = EntityType::load($entity_type);
    $properties = $this->entity_type->getProperties();
    $this->property_names = array();
    foreach($properties as $p){
      $this->property_names[] = $p->getName();
    }
    
    parent::__construct($values, $entity_type);

    foreach($this->property_names as $pn){
      $value = $this->{$pn};
      unset($this->$pn);
      $this->{$pn} = $value;
    }
  }
  
  public function getEntityType(){
    return $this->entity_type;
  }
  
  public function __call($name, $arguments){
    $property_name = eck_from_camel_case($name);
    $property_name = str_replace("set_", "", $property_name);
    if(in_array($property_name, $this->property_names)){
      $this->{$property_name} = $arguments[0];
    }
  }

  public function __set($name, $value) {
    if (isset($this->property_names) && in_array($name, $this->property_names) && !$this->ignore_validation) {
      $property_type = $this->entity_type->getProperty($name)->getType();
      $property_type_class = eck_get_property_type_class($property_type);

      if ($property_type_class::validate($value)) {
        $this->property_values[$name] = $value;
      }
      else {
        throw new Exception("Invalid value {$value} for property {$name} of type {$property_type} in
        Entity type: {$entity_type_name}");
      }

    } else {
      $this->{$name} = $value;
    }
  }


  public function __get($name) {
    if (array_key_exists($name, $this->property_values)){
      return $this->property_values[$name];
    }
    else if (isset($this->{$name})){
      return $this->{$name};
    }
    else if ($field = field_info_field($name)) {
      if ($this->entityType) {
        if ($bundle = $this->bundle()) {
          if (in_array($bundle, $field['bundles'][$this->entityType])) {
            $this->{$name} = array();
            return $this->{$name};
          }
        }
      }
    }
    return NULL;
  }

  public function __isset($name) {
    if(array_key_exists($name, $this->property_values)){
      return TRUE;
    }else if(isset($this->{$name})){
      return TRUE;
    }
    return FALSE;
  }

  public function save(){
    //going back to public properties
    $this->ignore_validation = TRUE;
    foreach($this->property_values as $property => $value){
      $this->{$property} = $value;
    }
    return parent::save();
  }
  
  public static function getRequirements(){
    global $eck_system;
    $entity_type = $eck_system->getFromContext('entity_type');
    $object = $eck_system->getMainObject();
    
    $ps = $entity_type->getProperties();
    $properties = new \ECK\Core\Properties();
    foreach($ps as $p){
      if($object){
        $p->setDefaultValue($object->{$p->getName()});
      }
      $properties->addProperty($p);
    }
    return $properties;
  }
  
  public static function loadAll(\ECK\Core\EntityType $entity_type){
    return entity_load($entity_type->getName());
  }
  
  public static function loadFromId($id){
    $pieces = explode("|", $id);
    return entity_load_single($pieces[0], $pieces[1]);
  }
  
  public static function load($entity_type, $id){
    return entity_load_single($entity_type->getName(), $id);
  }
  
  public function getName(){
    return $this->id;
  }
}
