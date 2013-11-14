<?php

/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 * we mainly have 2 types of hooks in drupal, the ones that affect an object and 
 * are related, to the life cycle of an entity in our case, or hooks to give information
 * or configure stuff
 */
namespace ECK\Behaviors;
class Behave {
  //put your code here
  public static function behave($method, $args){
    
    if(array_key_exists('entity', $args)){
      $entity = $args['entity'];
      $entity_type = $entity->getEntityType();
      $properties = $entity_type->getProperties();
      
      foreach($properties as $property){
        $behavior = $property->getBehavior();
        
        if($behavior){
          $behavior_info = eck_get_behavior_info($behavior->getType());
          if(array_key_exists('methods', $behavior_info) 
                  && in_array($method, $behavior_info['methods'])){
            $return = $behavior->behave($method, $args);
            if($return){
              $entity->{$property->getName()} = $return;
              $entity->is_new = FALSE;
              $entity->save();
            }
          }
        }
      }
    }
  }
  
}

?>
