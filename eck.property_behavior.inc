<?php

/**
 * A general function to call specific functions in a property behavior plugin
 */
function eck_property_behavior_invoke_plugin($entity_type, $function_name, $all = array(), 
 $specific = array()){
  
  $properties = $entity_type->properties;
  $returns = array();
  
  foreach($properties as $property => $info){
    //If there is a behavior associated with this property we need to call the appropiate hooks
    if(array_key_exists('behavior', $info) && !empty($info['behavior'])){
      $behavior = $info['behavior'];
     
      $plugin = ctools_get_plugins('eck', 'property_behavior', $behavior);
      
      $function = ctools_plugin_get_function($plugin, $function_name);
  
      if($function){
        if(array_key_exists($property, $specific)){
          $return = $function($property, array_merge($all, $specific[$property]));
        }else{
          $return = $function($property, $all);
        }
        
        if($return){
          $returns[$property] = $return;
        }
      }
    }
  }
  
  return $returns;
}
 
function eck_property_behavior_invoke_plugin_alter($entity_type, $function_name, $var){
  
  $properties = $entity_type->properties;
  $return = $var;
  
  foreach($properties as $property => $info){
    //If there is a behavior associated with this property we need to call the appropiate hooks
    if(array_key_exists('behavior', $info) && !empty($info['behavior'])){
      $behavior = $info['behavior'];
     
      $plugin = ctools_get_plugins('eck', 'property_behavior', $behavior);
      
      $function = ctools_plugin_get_function($plugin, $function_name);
  
      if($function){
        $return = $function($property, $var);
      }
    }
  }
  
  return $return;
  
}

