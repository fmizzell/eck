<?php

function eck_get_property_label($name){
  $info = hook_eck_property_info();
  return $info[$name]['label'];
}

function eck__property_edit__form($form, &$state, $entity_type_name, $property){
  $entity_type = EntityType::loadByName($entity_type_name);
  $properties = $entity_type->properties;
  if(array_key_exists($property, $properties)){
    $form['entity_type'] = array(
      '#type' => 'value',
      '#value' => $entity_type
    );
    
    $form['property'] = array(
      '#type' => 'value',
      '#value' => $property
    );
    
    $form['label'] = array(
      '#type' => 'textfield',
      '#title' => 'label',
      '#default_value' => $properties[$property]['label']
    );
    
    $form['submit'] = array(
      '#type' => 'submit',
      '#weight' => 10000,
      '#value' => t('Save'),
    );
  }else{
    drupal_set_message('This property does not exists');
    drupal_goto(eck__entity_type__path()."/{$entity_type->name}/property");
  }
  
  return $form;
}

function eck__property_edit__form_submit($form, &$state){
  $values = $state['values'];
  $entity_type = $values['entity_type'];
  $property = $values['property'];
  $label = $values['label'];
  
  $properties = $entity_type->properties;
  $properties[$property]['label'] = $label;
  
  $entity_type->properties = $properties;
  
  $entity_type->save();
  
  EntityType::loadAll(NULL, TRUE);
  $state['redirect'] = eck__entity_type__path()."/{$entity_type->name}/property";
}

function eck__property_delete__form($form, &$state, $entity_type_name, $property){
  $entity_type = EntityType::loadByName($entity_type_name);
  $properties = $entity_type->properties;
  if(array_key_exists($property, $properties)){
    $form['entity_type'] = array(
      '#type' => 'value',
      '#value' => $entity_type
    );
    
    $form['property'] = array(
      '#type' => 'value',
      '#value' => $property
    );
    
    $form['confirmation'] = confirm_form($form, 
     "Are you sure you want to delete {$property}", 
     eck__entity_type__path()."/{$entity_type->name}/property");
    
    
  }else{
    drupal_set_message('This property does not exists');
    drupal_goto(eck__entity_type__path()."/{$entity_type->name}/property");
  }
  
  return $form;
}

function eck__property_delete__form_submit($form, &$state){
  $values = $state['values'];
  $entity_type = $values['entity_type'];
  $property = $values['property'];
  
  $entity_type->removeProperty($property);
  
  $entity_type->save();
  
  EntityType::loadAll(NULL, TRUE);
  $state['redirect'] = eck__entity_type__path()."/{$entity_type->name}/property";
}



function eck_properties_property_config($form, &$state){
  return $form['eck_property_config2'];
}

function eck__properties__form_validate($form, &$state){
  if($state['values']['op'] == t('Add Property')){
    if (isset($state['values']['property_behavior'])) {
      $property_behavior = $state['values']['property_behavior'];
      $behavior_plugins = ctools_get_plugins('eck', 'property_behavior');
      if (isset($behavior_plugins[$property_behavior]['unique']) && $behavior_plugins[$property_behavior]['unique']) {
        foreach ($state['values']['new_properties'] as $property_name => $property_info) {
          if ($property_info['behavior'] == $property_behavior) {
            form_set_error('property_behavior', t('There can be only one property with \'@name\' behavior per entity.',
              array('@name' => $behavior_plugins[$property_behavior]['label'])));
          }
        }
      }
    }
  }
}

function eck__properties__form_submit($form, &$state){
  if($state['values']['op'] == t('Add Property')) {
    $state['rebuild'] = TRUE;
  }
  else if($state['values']['op'] == t('Save')) {
    //Here we want to add the properties to the entity type and save it
    $entity_type = $state['values']['entity_type'];
    //dpm($entity_type, "Entity Type");
    
    foreach($state['values']['new_properties_table'] as $property => $active){
      if($active){
        $info = $state['values']['new_properties'][$property];
        if(array_key_exists('behavior', $info)){
          $entity_type->addProperty($property, $info['label'], $info['type'], $info['behavior']);
        }else{
          $entity_type->addProperty($property, $info['label'], $info['type']);
        }
      }else{
        $entity_type->removeProperty($property);
      }
    }
    
    $entity_type->save();
    
    //lets flush the cache so new behaviors and properties will get set up correctly
    drupal_flush_all_caches();
  }
}