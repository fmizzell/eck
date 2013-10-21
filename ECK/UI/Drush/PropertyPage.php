<?php
namespace ECK\UI\Drush;
use ECK\Core\EntityType;

class PropertyPage{
  public static function listing($entity_type_name){
    $entity_type = EntityType::load($entity_type_name);
    $rows = array();
    $rows[] = array('Name', 'Label', 'Type', 'Behavior');
    foreach($entity_type->getProperties() as $property){
      $rows[] = array($property->getName(), $property->getLabel(),$property->getType(), 
          $property->getBehavior());
    }
    
    drush_print_table($rows, TRUE);
  }
  
  public static function add($entity_type_name){
    $entity_type = EntityType::load($entity_type_name);
    
    $name = drush_prompt("Name", "");
    $type = drush_prompt("Type", "");
    
    $entity_type->addProperty($name, $type);
    $entity_type->save();
  }
  
  /**
   * @param (string) $property_id a string with the format entity_type_name:property_name
   */
  public static function edit($property_id){
    $pieces = explode(":", $property_id);
    
    $entity_type = EntityType::load($pieces[0]);
    $property = $entity_type->getProperty($pieces[1]);
    
    $label = drush_prompt("Label", $property->getLabel());
    
    $property->setLabel($label);
    $entity_type->save(); 
  }
  
  public static function delete($property_id){
    $pieces = explode(":", $property_id);
    
    $entity_type = EntityType::load($pieces[0]);
    $property = $entity_type->getProperty($pieces[1]);
    
    if($entity_type){
      $delete = drush_confirm("Are you sure you want to delete the property {$property->getName()}
        in {$entity_type->getName()}?");
      if($delete){
        $entity_type->removeProperty($pieces[1]);
        $entity_type->save();
        drush_log("property has been deleted", 'success');
      }
    }else{
      drush_log("Entity type {$entity_type_name} does not exist!", "error");
    }
  }
  
  public static function addBehavior($entity_type){
    return "Hello";
  }
  
  public static function removeBehavior($entity_type){
    return "Hello";
  }
}
