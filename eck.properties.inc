<?php

/**
 * Returns all default properties for ECK controlled entities.
 */
function eck_default_properties() {
  return array(
    'uuid' => t('UUID'),
    'uid' => t('Author'),
    'created' => t('Created'),
    'changed' => t('Changed'),
    'state' => t('State')
  );
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
function eck__property__schema(&$schema, $entity_type, $property_name) {

  switch ($property_name) {
    case 'uuid':
      // Prepend UUID field.
      $schema['fields']['uuid'] = array(
        'type' => 'char',
        'length' => 36,
        'not null' => TRUE,
        'default' => '',
        'description' => 'The Universally Unique Identifier.'
      );
      break;

    case 'uid':
      // Append author id.
      $schema['fields']['uid'] = array(
        'description' => "The {users}.uid that owns this {$entity_type->name}; initially, this is the user that created it.",
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
        'initial' => $GLOBALS['user']->uid,
      );
      $schema['indexes']['uid'] = array('uid');
      $schema['foreign keys']["{$entity_type->name}_author"] = array(
        'table' => 'users',
        'columns' => array('uid' => 'uid'),
      );
      break;

    case 'created':
      // Append created timestamp.
      $schema['fields']['created'] = array(
        'description' => "The Unix timestamp when the {$entity_type->name} was created.",
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
        'initial' => REQUEST_TIME,
      );
      $schema['indexes']["{$entity_type->name}_created"] = array('created');
      break;

    case 'changed':
      // Append changed timestamp.
      $schema['fields']['changed'] = array(
        'description' => "The Unix timestamp when the {$entity_type->name} was most recently saved.",
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
        'initial' => REQUEST_TIME,
      );
      $schema['indexes']["{$entity_type->name}_changed"] = array('changed');
      break;
      
    case 'state':
      $schema['fields']['state'] = array(
         'description' => "{$entity_type->name} state",
         'type' => 'int',
         'unsigned' => TRUE,
         'not null' => TRUE,
         'default' => 0
      );
      break;
    }
}

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
  
 
  foreach (eck_default_properties() as $property => $label) {
    $form['properties'][$property] = array(
      '#type' => 'checkbox',
      '#title' => $label,
      '#default_value' => 
      array_key_exists($property, $entity_type->properties) ? $entity_type->properties[$property] : 1
    );
  }
  
  return $form;
}

/**
 * Get the property's schema
 */
function eck__custom_property__schema($property_type, $property_name) {
  $schema = array();
  $schema[$property_name] = array();
  $spec = $schema[$property_name];
  
  switch ($property_type) {
    case 'text':
      $spec = array(
        //'description' => '',
        'type' => 'varchar',
        'length' => 255,
        'not null' => TRUE,
        'default' => '',
      );
      break;

    case 'decimal':
      $spec = array(
        //'description' => '',
        'type' => 'float',
        'not null' => TRUE,
        'default' => 0,
      );
      break;

    case 'integer':
    case 'date':
    default:
      // Integer, date, and entities are all of type 'int'.
      $spec = array(
        //'description' => '',
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      );
      break;
  }

  return $schema;
}

function eck__custom_property__form($entity_type){
  $form = array();
  
  // Enable/disable custom properties.
  $form['custom_properties'] = array(
    '#type' => 'fieldset',
    '#title' => t('Custom properties'),
    '#description' => t('Note: unchecking will delete the property and all data stored within!'),
    '#access' => !empty($entity_type->custom_properties),
    '#collapsible' => TRUE,
    '#tree' => TRUE,
  );

  foreach ($entity_type->custom_properties as $property => $info) {
    $form['custom_properties'][$property] = array(
      '#type' => 'checkbox',
      '#title' => eck__entity_type__custom_property_label($info),
      '#default_value' => 1,
    );
  }

  // Add custom property.
  $form['add'] = array(
    '#type' => 'fieldset',
    '#title' => t('Add custom property'),
    '#access' => empty($entity_type->is_new),
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
  foreach (entity_get_info() as $name => $data) {
    $types[t('Entities')][$name] = $data['label'];
  }

  $form['add']['type'] = array(
    '#type' => 'select',
    '#title' => t('Type'),
    '#options' => array('' => t('- Please choose -')) + $types,
  );

  $form['add']['label'] = array(
    '#type' => 'textfield',
    '#title' => t("Name"),
    '#description' => t("A human readable name for the property."),
  );

  $form['add']['name'] = array(
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
