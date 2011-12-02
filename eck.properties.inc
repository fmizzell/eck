<?php

function eck_set_properties_schema(&$schema, $entity_type){
  $properties = $entity_type->properties;
  $property_info = hook_eck_property_info();
  
  foreach($properties as $name => $info){
    if(array_key_exists('custom', $info)){
      $type = $info['custom'];
      $schema['fields'][$name] = eck_general_property_schema($type);
      //need to generate custom properties schema
    }else{
      $schema['fields'][$name] = $property_info[$name]['schema'];
    }
  }
  
}

function eck_general_properties(){
  return array('text', 'decimal', 'integer');
}

function eck_general_property_schema($type){
  $schema = array();
  
  switch ($type) {
    case 'text':
      $schema = array(
        'description' => 'Text',
        'type' => 'varchar',
        'length' => 255,
        'not null' => TRUE,
        'default' => '',
      );
      break;

    case 'decimal':
      $schema = array(
        'description' => 'Decimal',
        'type' => 'float',
        'not null' => TRUE,
        'default' => 0,
      );
      break;

    case 'integer':
    //case 'date':
    //default:
    // Integer, date, and entities are all of type 'int'.
      $schema = array(
        'type' => 'int',
        'description' => "Integer",
        'not null' => TRUE,
        'default' => 0,
      );
      break;
    
    default:
      $schema = NULL;
      break;
  }
  return $schema;
}


/**
 * Generates schema definition for additional ECK properties.
 *
 * @param $property_name
 *   Property to generate schema definition for.
 * @param $entity_type
 *   (Object) An object with and entity types info, as returned by entity_type__load 
 * This is only use to add stuff to the descriptions of the property, do we really need this?
 * Well, if people is already using the module it will be a pain in the butt to syncronize their schemas to use
 * indexes, and so on without the specifics of the entity type, so for the sake of moving forward to more important
 * things, lets leave the $entity_type as a parameter for this function.
 */
/*function eck__property__schema(&$schema, $entity_type, $property_name) {
}*/

function eck__property__form($entity_type){
  $form = array();
  // Enable/disable properties.
  $form['properties'] = array(
    '#type' => 'fieldset',
    '#title' => t('Properties'),
    '#description' => t('Note: unchecking will delete the property and all data stored within!'),
    '#collapsible' => TRUE,
    '#tree' => TRUE,
  );
  
  $properties = hook_eck_property_info();
 
  foreach ($properties as $property => $info) {
    
      $form['properties'][$property] = array(
        '#type' => 'checkbox',
        '#title' => $info['label'],
        '#default_value' => 
        isset($entity_type->properties) &&
        array_key_exists($property, $entity_type->properties) ? 1 : 0
      );
  }
  
  return $form;
}

/**
 * Get the property's schema
 */
/*function eck__custom_property__schema($property_type, $property_name) {
}*/

function eck__custom_property__form($entity_type){
  $form = array();
  
  $properties = $entity_type->properties;
  $custom_properties = array();
  
  foreach($properties as $property => $info){
    if(array_key_exists('custom', $info) && $info['custom']){
      $custom_properties[$property] = $info;
    }
  }
  
  // Enable/disable custom properties.
  $form['custom_properties'] = array(
    '#type' => 'fieldset',
    '#title' => t('Custom properties'),
    '#description' => t('Note: unchecking will delete the property and all data stored within!'),
    '#access' => !empty($custom_properties),
    '#collapsible' => TRUE,
    '#tree' => TRUE,
  );

  foreach ($custom_properties as $property => $info) {
    $form['custom_properties'][$property] = array(
      '#type' => 'checkbox',
      '#title' => $info['label'],
      '#default_value' => 1,
    );
  }

  // Add custom property.
  $form['add_custom_property'] = array(
    '#type' => 'fieldset',
    '#title' => t('Add custom property'),
    '#access' => !$entity_type->is_new,
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#tree' => TRUE,
  );

  $types[t('Generic')] = array(
    'text' => t('Text'),
    'decimal' => t('Decimal'),
    'integer' => t('Integer'),
    'date' => t('Timestamp'),
  );
  
  /*foreach (entity_get_info() as $name => $data) {
    $types[t('Entities')][$name] = $data['label'];
  }*/

  $form['add_custom_property']['type'] = array(
    '#type' => 'select',
    '#title' => t('Type'),
    '#options' => array('' => t('- Please choose -')) + $types,
  );

  $form['add_custom_property']['label'] = array(
    '#type' => 'textfield',
    '#title' => t("Name"),
    '#description' => t("A human readable name for the property."),
  );

  $form['add_custom_property']['name'] = array(
    '#type' => 'machine_name',
    '#required' => FALSE,
    '#machine_name' => array(
      'label' => t("Database column name"),
      'exists' => '_eck_fake_exists',
      'source' => array('add', 'label'),
    ),
  );
  
  return $form;
  
}
