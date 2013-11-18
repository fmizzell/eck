<?php
namespace ECK\Core;
use ECK\Core\Property;
class EntityProperty extends Property {
  
  //This gets control by the entity type, that way we know whether this property
  //needs to be added to the entity type, or if it already exists so it will
  //be save by reference
  private $is_new;
  
  private $entity_type;
  private $behavior;
  private $schema;
  
  public function __construct(EntityType $et, $name, $type){
    parent::__construct($name, $type);
    $this->entity_type = $et;
    $this->behavior = NULL;
    $this->schema = array();
    
    $this->is_new = TRUE;
  }
  
  public function getIdentifier(){
    return $this->getName();
  }
  
  public function isNotNew(){
    $this->is_new = FALSE;
  }
  
  /**
   * There is a base schema defined by the type of the property,
   * but we allow modifications of the base schema
   */
  public function setSchema($schema){
    $this->schema = $schema;
  }
  
  public function getSchema(){
    $schema = array();
    $property_type_class = eck_get_property_type_class($this->type);
    $base_schema = $property_type_class::schema();
    if(!empty($this->schema)){
      //if the default was empty, then we took it off the property schema, so we need
      //to make sure we don't add it back during the merge
      if(array_key_exists('default', $base_schema) && 
              !array_key_exists('default', $this->schema)){
        unset($base_schema['default']);
      }
      
      $schema = array_merge($base_schema, $this->schema);
    }else{
      $schema = $base_schema;
    }
    return $schema;
  }
  
  public function setBehavior($behavior){
    $this->behavior = $behavior;
  }
  
  public function getBehavior(){
    return $this->behavior;
  }
  
  /**
   * Create a json version of the object
   */
  public function serialize(){
    $array = array();
    $properties = array('name', 'type', 'label', 'schema');
    foreach($properties as $p){
      $array[$p] = $this->{$p};
    }
    
    if($this->behavior){
      $array['behavior'] = $this->behavior->serialize();
    }
    
    if($this->widget){
      $array['widget'] = $this->widget->serialize();
    }
    
    if($this->formatter){
      $array['formatter'] = $this->formatter->serialize();
    }

    return drupal_json_encode($array);
  }
  
  /*
   * Given a json version of a Permissions object,
   * return a proper property object
   */
  static public function deserialize(EntityType $et, $string){
    $a = drupal_json_decode($string);
    if(array_key_exists('name', $a) && array_key_exists('type', $a)){
      $property = new EntityProperty($et, $a['name'], $a['type']);
      $property->setLabel($a['label']);
      $property->setSchema($a['schema']);
      
      if(array_key_exists('behavior', $a)){
        $property->setBehavior(\ECK\Core\PropertyBehavior::deserialize($property, $a['behavior']));
      }
      
      if(array_key_exists('widget', $a)){
        $property->setWidget(\ECK\Core\PropertyWidget::deserialize($property, $a['widget']));
      }
      
      if(array_key_exists('formatter', $a)){
        $property->setFormatter(\ECK\Core\PropertyFormatter::deserialize($property, $a['formatter']));
      }

      return $property;
    }
    
    return NULL;
  }
  
  public function save(){
    if($this->is_new){
      $this->entity_type->addPropertyObject($this);
    }
    $this->entity_type->save();
  }
  
  public function delete(){
    $this->entity_type->removeProperty($this->getName());
    $this->entity_type->save();
  }
  
  public static function loadFromId($id){
    $pieces = explode("|", $id);
    $et = EntityType::loadFromId($pieces[0]);
    return $et->getProperty($pieces[1]);
  }
  
  public static function load($entity_type, $id){
    return $entity_type->getProperty($id);
  }
  
  public static function loadAll(\ECK\Core\EntityType $entity_type){
    return $entity_type->getProperties();
  }
  
  public static function getMetaProperty($property_name, EntityProperty $property = NULL){
    
    if($property_name == "name"){
      $p = new Property('name', new \ECK\PropertyTypes\Text());
      $p->setLabel = "Name";
      $p->required();
      $p->setWidget(new \ECK\UI\Widgets\Text($p));
      
      if($property){
        $p->setDefaultValue($property->getName());
      }
      
      return $p;
    }else if ($property_name == "type"){
      $p = new Property('type', new \ECK\PropertyTypes\Text());
      $p->setLabel = "Type";
      $p->setWidget(new \ECK\UI\Widgets\Text($p));
      
      if($property){
        $p->setDefaultValue($property->getType());
      }
      
      return $p;
    }
    
    else if ($property_name == "label"){
      $p = new Property('label', new \ECK\PropertyTypes\Text());
      $p->setLabel = "Label";
      $p->setWidget(new \ECK\UI\Widgets\Text($p));
      
      if($property){
        $p->setDefaultValue($property->getLabel());
      }
      
      return $p;
    }
    
    return NULL;
  }
}
