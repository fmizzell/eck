<?php

/**
 * As you can see this is just use to comply with requirements
 * probably no one will use this
 */

function eck_entity_property_info(){
  module_load_include('inc', 'entity', 'entity.info');
  //dpm("In ECK entity property info");
  $info = array();
  // Add meta-data about the basic node properties.
  //$properties = &$info['entity_type']['properties'];
  foreach(EntityType::loadAll() as $entity_type){
    
    $properties = $entity_type->properties;
    $stuff = entity_metadata_convert_schema("eck_{$entity_type->name}");
    foreach($stuff as $key => $property){
      // Use user defined label for property
      if (isset($properties[$key])) {
        $property['label'] = $properties[$key]['label'];
      }
      $property['setter callback'] = "entity_property_verbatim_set";
      $property['getter callback'] = 'entity_property_verbatim_get';
      $property['description'] = $property['label'];
      //A couple of alter hooks so a module can alter the property info
      //of a given property, or even a specific property on a
      //specific entity_type
      drupal_alter("entity_property_{$key}_info", $property);
      drupal_alter("entity_property_{$entity_type->name}_{$key}_info", $property);
      
      if ($key == 'type') {
        $property['label'] = t('!entity_type type', array('!entity_type' => $entity_type->name));
        $property['type']  = 'token';
        $property['description'] = t('The type of this :entity_type entity.', array(':entity_type' => $entity_type->name));
        $property['options list'] = 'EntityDefaultMetadataController::bundleOptionsList';
        $property['required'] = TRUE;
      }
      $stuff[$key] = $property;
    }
    $info[$entity_type->name]['properties'] = $stuff;
  }
  
  return $info;  
}

function eck_entity_property_info_alter(&$info){
  
  foreach (EntityType::loadAll() as $entity_type) {
    $entity_property_info = $info[$entity_type->name];
    
    $entity_property_info =
    eck_property_behavior_invoke_plugin_alter($entity_type, 'property_info',$entity_property_info);
    
    foreach($entity_type->properties as $property => $stuff){
      foreach(array('setter', 'getter', 'validation') as $function_name){
        if(eck_property_behavior_implements($entity_type, $property, $function_name)){
          $entity_property_info['properties'][$property] ["{$function_name} callback"] 
            = "eck_property_behavior_{$function_name}";
        }
      }
    }

    if($entity_property_info){
      $info[$entity_type->name] = $entity_property_info;
    }
  }
}