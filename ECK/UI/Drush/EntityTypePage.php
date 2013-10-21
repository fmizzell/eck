<?php

namespace ECK\UI\Drush;
use ECK\Core\EntityType;

class EntityTypePage{
  public static function listing(){
    $rows = array();
    $rows[] = array('Name', "Label");
    foreach(EntityType::loadAll() as $et){
      $rows[] = array($et->getName(), $et->getLabel());
    }
    
    drush_print_table($rows, TRUE);
  }
  
  public static function add(){
    $name = drush_prompt("Name", "");
    $label = drush_prompt("Label", "");
    
    $entity_type = new EntityType($name);
    $entity_type->setLabel($label);
    $entity_type->save();
  }
  
  public static function edit($entity_type_name){
    $entity_type = EntityType::load($entity_type_name);
    if($entity_type){
      $label = drush_prompt("Label", $entity_type->getLabel());
      $entity_type->setLabel($label);
      $entity_type->save();
      drush_log("label changed", 'success');
    }else{
      
    }
  }
  
  public static function delete($entity_type_name){
    $entity_type = EntityType::load($entity_type_name);
    if($entity_type){
      $delete = drush_confirm("Are you sure you want to delete {$entity_type_name}?");
      if($delete){
        $entity_type->delete();
        drush_log("entity types has been deleted", 'success');
      }
    }else{
      drush_log("Entity type {$entity_type_name} does not exist!", "error");
    }
  }
}

