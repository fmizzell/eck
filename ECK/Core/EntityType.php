<?php
namespace ECK\Core;

use ECK\Core\ObjectStorer;
use ECK\Core\DBTableTransformer;
use ECK\Core\Property;

class EntityType{ 
  /**
   * The db table transformer is charged with translating entity type and property
   * operations into actual changes to the db table associeted with storing
   * the entities of this entity type.
   * @var DBTableTransformer object
   */
  private $dbtt;
  
  private $name;
  
  private $label;
  
  private $properties;
  
  public function __construct($name){
    $this->name = $name;
    $this->label = eck_labelize($name);
    $this->properties = array();
    $this->dbtt = new DBTableTransformer("eck_{$name}", $this->getSchema());
  }
  
  public function getSchema(){
    $schema = array(
      'fields' => array(
        'id' => array(
          'type' => 'serial',
          'not null' => TRUE,
        )
      ),
      'primary key' => array('id'),
    );
    
    foreach($this->getProperties() as $p){
      $schema['fields'][$p->getName()] = $p->getSchema();
    }
    
    return $schema;
  }
  
  public function setName($name){
    if(empty($this->name)){
      $this->name = $name;
    }
  }
  
  public function getName(){
    return $this->name;
  }
  
  public function setLabel($label){
    $this->label = $label;
  }
  
  public function getLabel(){
    return $this->label;
  }
  
  /**
   * This function, even though is public, is only useful when an entity type
   * is being loaded from the db
   * @param (string) $properties a json string representing a property
   */
  public function deserializeProperties($properties){
    $properties = drupal_json_decode($properties);
    foreach($properties as $p){
      $property = EntityProperty::deserialize($this, $p);
      $property->isNotNew();
      if($property){
        $this->properties[] = $property;
      }
    }
  }
  
  /**
   * This function, even though is public, is only useful when an entity type
   * is being saved to the db
   * @param type $properties 
   */
  public function getProperties(){
    return $this->properties;
  }
  
  public function serializeProperties(){
    $properties = array();
    foreach($this->properties as $p){
      $properties[] = $p->serialize();
    }
    $json = drupal_json_encode($properties);
    return $json;
  }
  
  static public function loadAll(){
    $os = new ObjectStorer("ECK\\Core\\EntityType", array('name') ,"eck_entity_type");
    return $os->load();
  }
  
  static public function load($name){
    $os = new ObjectStorer("ECK\\Core\\EntityType", array('name') ,"eck_entity_type");
    $entity_types = $os->load('name', $name);
    return array_shift($entity_types);
  }
  
  public function save(){
    $os = new ObjectStorer("ECK\\Core\\EntityType", array('name') ,"eck_entity_type", 'eck_entity_type');
    $return = $os->save($this);
    if($return == OBJECT_STORER_CREATED){
      $this->dbtt->createTable();
    }
    $this->dbtt->performTransformations();
  }
  
  public function delete(){
    $os = new ObjectStorer("ECK\\Core\\EntityType", array('name') ,"eck_entity_type", 'eck_entity_type');
    $os->delete($this);
    $this->dbtt->deleteTable();
    $this->dbtt->performTransformations();
  }
  
  public function getProperty($name){
    foreach($this->properties as $key => $property){
      if($property->getName() == $name){
        return $property;
      }
    }
    
    return NULL;
  }
  
  public function addProperty($name, $type){
    $p = new EntityProperty($this, $name, $type);
    $this->properties[] = $p;
    $this->dbtt->addField($name, $p->getSchema());
  }
  
  public function addPropertyObject(\ECK\Core\EntityProperty $property){
    $this->properties[] = $property;
    $this->dbtt->addField($property->getName(), $property->getSchema());
  }
  
  public function removeProperty($name){
    foreach($this->properties as $key => $property){
      if($property->getName() == $name){
        unset($this->properties[$key]);
        $this->dbtt->removeField($name);
      }
    }
  }
  
  public static function getMetaProperty($property_name, EntityType $entity_type = NULL){
    
    if($property_name == "name"){
      $p = new Property('name', new \ECK\PropertyTypes\Text());
      $p->setLabel = "Name";
      $p->required();
      $p->setWidget(new \ECK\UI\Widgets\Text($p));
      
      if($entity_type){
        $p->setDefaultValue($entity_type->getName());
      }
      
      return $p;
    }else if ($property_name == "label"){
      $p = new Property('label', new \ECK\PropertyTypes\Text());
      $p->setLabel = "Label";
      $p->setWidget(new \ECK\UI\Widgets\Text($p));
      
      if($entity_type){
        $p->setDefaultValue($entity_type->getLabel());
      }
      
      return $p;
    }
    
    return NULL;
  }
  
  public static function loadFromId($id){
    return EntityType::load($id);
  }

  /*
  public function changeBehavior($name, $behavior){
    $p = $this->properties;
    //@todo check that type is an actual type
    if(array_key_exists($name, $p)){
      $p[$name]['behavior'] = $behavior;
      //@todo look at this more closelly, does the behavior change really affect the property
      //cache?
      entity_property_info_cache_clear();
    }else{
      //@Todo add exception.. the property does not exist
    }

    $this->properties = $p;
  }

  public function removeBehavior($name){
    $this->changeBehavior($name, NULL);
  }*/
}
