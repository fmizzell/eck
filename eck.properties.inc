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
 * @param $&schema
 *   Schema definition.
 * @param $property
 *   Property to generate schema definition for.
 */
function eck_add_property_schema(&$schema, $entity_type, $property) {
  if (!isset($schema['fields'])) {
    $schema['fields'] = array();
  }

  switch ($property) {
    case 'uuid':
      // Prepend UUID field.
      $schema['fields'] = array('uuid' => array(
        'type' => 'char',
        'length' => 36,
        'not null' => TRUE,
        'default' => '',
        'description' => 'The Universally Unique Identifier.'
      )) + $schema['fields'];
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

function eck_get_custom_property_schema($property) {
  switch ($property['type']) {
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
  return $spec;
}