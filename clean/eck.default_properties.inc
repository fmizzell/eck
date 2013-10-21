<?php

function eck_eck_default_properties(){
  $default_properties = array();
  
  $default_properties['title'] =
  array(
    'label' => "Title",
    'type' => "text",
    'behavior' => 'title',
    'form' =>
    array(
      'label' => "Title",
      'description' => "",
      'widget' => array(
        'type' => 'text',
         'settings' => array(
           'size' =>  60,
           'max_length' => 255,
         ) 
      ),
      'default_value' => ''
    ),
    'display' => array(
      'theme' => 'eck_basic_formatter',
      'settings' => array()
    )
  );
  
  $default_properties['uid'] =
  array(
    'label' => "Author",
    'type' => "integer",
    'behavior' => 'author'
  );
  
  $default_properties['created'] =
  array(
    'label' => "Created",
    'type' => "integer",
    'behavior' => 'created'
  );
  
  $default_properties['changed'] =
  array(
    'label' => "Changed",
    'type' => "integer",
    'behavior' => 'changed'
  );
  $default_properties['language'] =
  array(
    'label' => "Entity language",
    'type' => "language",
    'behavior' => 'language'
  );
  
  return $default_properties;
}

function eck__is_default_property($property){
  $default_properties = eck_get_default_properties();
  $is_default = FALSE;
  
  if(array_key_exists($property, $default_properties)){
    $is_default = TRUE;
  }
  
  return $is_default;
}

function eck__activate_default_property($entity_type_name, $property){
  $entity_type = EntityType::loadByName($entity_type_name);
  if(eck__is_default_property($property)){
    //we have 2 cases, if it is active, do nothing
    $properties = $entity_type->properties;
    if(array_key_exists($property, $properties)){
      drupal_set_message("{$property} is already active");
    }
    //else activate the property
    else{
      $info = eck_get_default_property($property);
      $entity_type->addProperty($property, $info['label'], $info['type'], 
        $info['behavior']);
      $entity_type->save();
      
      if(!empty($info['form'])){
        $bundles = Bundle::loadByEntityType($entity_type);
        foreach($bundles as $bundle){
          $config = $bundle->config;
          $config['extra_fields'][$property]['form'] = $info['form'];
          $bundle->config = $config;
          $bundle->save();
        }
      }
      
      EntityType::loadAll(NULL, TRUE);
      Bundle::loadAll(NULL, TRUE);
    }
  }else{
    drupal_set_message("{$property} is not a default property", "error");
  }
  drupal_goto(eck__entity_type__path()."/{$entity_type->name}/property");
}

function eck__deactivate_default_property($entity_type_name, $property){
  $entity_type = EntityType::loadByName($entity_type_name);
  if(eck__is_default_property($property)){
    //we have 2 cases, if it is active, do nothing
    $properties = $entity_type->properties;
    if(!array_key_exists($property, $properties)){
      drupal_set_message("{$property} is already inactive");
    }
    //else activate the property
    else{
      $entity_type->removeProperty($property);
      $entity_type->save();
      EntityType::loadAll(NULL, TRUE);
      Bundle::loadAll(NULL, TRUE);
    }
  }else{
    drupal_set_message("{$property} is not a default property", "error");
  }
  drupal_goto(eck__entity_type__path()."/{$entity_type->name}/property");
}


function  eck__default_properties__form($form, &$state, $entity_type){
   //DEFAULT PROPERTIES
  //Now we want to display the default properties
  $default_properties = eck_get_default_properties();
  $options = array();
  foreach($default_properties as $property_name => $property_info){
    $options[$property_name] = $property_info['label'];
  }
  
  $form['default_properties'] = array(
    '#type' => 'checkboxes',
    '#options' => $options,
    '#title' => t('Default Properties')
  );
  
  //Now we need to set the defaults if the entity_type has any
  $properties = $entity_type->properties;
  
  $defaults = array();
  foreach($properties as $property => $info){
    $defaults[$property] = $property;
  }
  
  $form['default_properties']['#default_value'] = $defaults;
  // :S
  
  return $form;
}

function  eck__default_properties__form_submit($form, &$state, $entity_type){
  
  $entity_type = $state['values']['entity_type'];
  $dp = $state['values']['default_properties'];
  foreach($dp as $property => $active){
    if($active){
      $info = eck_get_default_property($property);
      $entity_type->addProperty($property, $info['label'], $info['type'], 
              $info['behavior'], $info['schema']);
    }else{
      $entity_type->removeProperty($property);
    }
  }
  
  $state['values']['entity_type'] = $entity_type;
}

function eck_get_default_properties(){
  return module_invoke_all('eck_default_properties');
}

function eck_get_default_property($name){
  $default_properties = eck_get_default_properties();
  return $default_properties[$name];
}