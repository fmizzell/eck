<?php
use ECK\Core\EntityType;
/**
 * Sets up an entities form
 *
 * @param $form
 *  Form array provided by the Form API
 * @param $form_state
 *  array provided by the Form API
 * @param $entity
 *  an object as returned by entity_load()
 */
function eck__entity__edit_form($form, &$form_state, $entity) {
  
  $form['entity'] = array(
    '#type' => 'value',
    '#value' => $entity
  );
  
  // Property Widget Handling
  $entity_type = EntityType::load($entity->entityType());
  foreach($entity_type->getProperties() as $property){
    $property_widget = $property->getWidget();
    if($property_widget){
      $form[$property->getName()] = $property_widget;
    }
  }
  
  $form['submit'] = array(
    '#type' => 'submit',
    '#weight' => 10000,
    '#value' => t('Save'),
  );
  
  return $form;
  
  
  //$bundle = bundle_load($entity_type->name, $entity->type);
  
  
  
  /*$property_info = entity_get_property_info($entity->entityType());
  $language = LANGUAGE_NONE;
  if(function_exists("entity_language")){
    $language = entity_language($entity_type->name, $entity);
  }
  $properties = array();
    
  if (empty($bundle->config['extra_fields'][$property_name]['form']) &&
      empty($info['form'])) continue;
    
  if(!empty($info['form'])){
    $bundle_property_config = $info['form'];
  }
  //bundles can override entity type settings
  if(!empty($bundle->config['extra_fields'][$property_name]['form'])){
    $bundle_property_config = $bundle->config['extra_fields'][$property_name]['form'];
  }*/
    
  //$widget_type = eck_property_info_widget_types($bundle_property_config['widget']['type']);
    
    
  // Get the default value for this property.
  /*$value = NULL;
  if (isset($entity->{$property_name})) {
    $value = $entity->{$property_name};
  } elseif (isset($widget_config['default_value_function'])) {
    $value = $widget_config['default_value_function']($entity_type, $bundle, $entity);
  } elseif ($widget_config['default_value']) {
    $value = $widget_config['default_value'];
  }*/

  // Include external module file dependency if one is required.
  /*if (function_exists('drupal_get_path') && $widget_type['file']) {
    form_load_include($form_state, $widget_type['file type'], $widget_type['module'], $widget_type['file']);
  }*/

  //$function = $widget_type['module'] . '_eck_property_widget_form';
  //if (TRUE){//function_exists($function)) {

    /**/
    // Call the widget's form hook and load the widget form element.
    /*if (TRUE){//element = $function($form, $form_state, $property_name, $bundle_property_config, $language, $value, $element)) {
      // Allow modules to alter the property widget form element.
      $context = array(
        'form' => $form,
        'property_name' => $property_name,
        'config' => $widget_config,
        'langcode' => LANGUAGE_NONE,
        'value' => $value,
      );
      drupal_alter(array('eck_property_widget_form', 'eck_property_widget_' . $widget_type . '_form'), $element, $form_state, $context);
    }
    //$properties[$property_name] = $element;
  }*/

  //field_attach_form($entity->entityType(), $entity, $form, $form_state);
}

/**
 * Validation function for entity form for validating the fields
 *
 * @param $form
 *  Form array provided by the Form API
 * @param $form_state
 *  array provided by the Form API
 */
function eck__entity__edit_form_validate($form, &$state) {
  $entity = $state['values']['entity'];
  //field_attach_form_validate($entity->entityType(), $entity, $form, $state);
  
  //lets validate our properties by trying to set them :)
  $entity_type_name = $entity->entityType();
  $entity_type = EntityType::load($entity_type_name);
  $properties = $entity_type->getProperties();
  
  //If we find a value set for a property lets just set it
  foreach($properties as $p){
    $form_value = _eck_form_property_value($state, $p->getName());
    
    if(isset($form_value)){
      
      //@TODO This should be a widget hook not a behavior function
      /*$vars = array('data' => $form_value);
      $data = eck_property_behavior_invoke_plugin($entity_type, 'pre_set', 
      $vars);
      
      if(array_key_exists($property, $data)){
        $form_value = $data[$property];
      }*/
      
      try{
        $entity->{$p->getName()} = $form_value;
      }catch(Exception $e){
        //if there was an exception lets set up the proper for error
        form_set_error($property, "Invalid property value {$form_value}, value should be of type {$info['type']}");
      }
    }
  } 
}

/**
 * Submit function for entity form
 *
 * @param $form
 *  Form array provided by the Form API
 * @param $form_state
 *  array provided by the Form API
 */
function eck__entity__edit_form_submit($form, &$state) {
  $entity = $state['values']['entity'];
  
  //field_attach_submit($entity->entityType(), $entity, $form, $state);
  
  $entity->save();

  drupal_set_message(t("Entity {$entity->id} - @entity_label has been saved", array("@entity_label" => entity_label($form['#entity_type'], $entity)) ));
  $uri = eck__entity__uri($entity);
  $state['redirect'] = $uri['path'];
}
