<?php
/**
 * 
 * @file
 * 
 * ENTITY TYPE
 * 
 * Entity Types represent types of data. Drupal core contains multiple
 * entity types nodes, users, vocabularies, etc.
 * 
 * ECK allows you to create entity types. This file contains all
 * of the entity type specific functionality
 * 
 */

/**
 * This function is calles from the main hook_men (eck_menu)
 * in eck.module. 
 * It creates the menu items related to entity type
 * management
 */
function eck__entity_type__menu() {
  $menu = array();

  //OVERVIEW Entity Type
  // View all of the created entity types
  $menu['admin/structure/eck'] = array(
    'title' => 'Entity Type',
    'description' => 'A centralized administrative section for entity types',
    'page callback' => 'eck__entity_type__overview',
    'access arguments' => array('administer entity types'),
    'file' => 'eck.entity_type.inc'
  );

  //ADD Entity
  $menu["admin/structure/eck/add"] = array(
    'title' => 'Add Entity Type',
    'description' => 'Add new entity type',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('eck__entity_type__add'),
    'access arguments' => array('add entity types'),
    'type' => MENU_LOCAL_ACTION,
    'weight' => 1,
    'file' => 'eck.entity_type.inc'
  );
  
  module_load_include('inc', 'eck', 'eck.bundle');
  //Each entity type can have multiple bundles
  //Now lets create the menus for the bundle administration of each
  //entity type
  foreach (eck__get_entity_types () as $entity_type) {
    $menu = array_merge($menu, eck__bundle__menu($entity_type));
  }

  return $menu;
}


/**
 * callback for the entity_type overview
 */
function eck__entity_type__overview() {

  $header = array(t('Name'), array('data' => t('Operations'), 'colspan' => '1'));
  $rows = array();

  $results = db_select('eck', 'e')->fields('e')->execute();

  foreach ($results as $record) {
    $id = $record->id;
    $name = $record->name;
    $label = $record->label;

    $row = array(l(t("{$id} : {$label}"), "admin/structure/eck/{$name}"));
    /* $row[] = array('data' => l(t('delete'), "admin/structure/eck/{$name}/delete")); */
    $rows[] = $row;
  }
  $build['entity_table'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
  );

  return $build;
}


/**
 * callback for adding entity types functionality
 */
function eck__entity_type__add($form, &$form_state) {

  $form['entity_type_label'] = array(
    '#type' => 'textfield',
    '#title' => "Entity Type",
    '#description' => "A human readable name for the entity type",
    '#required' => TRUE
  );

  $form['entity_type'] = array(
    '#type' => 'machine_name',
    '#machine_name' => array(
      'exists' => '_eck_fake_exists',
      'source' => array('entity_type_label'),
    ),
  );

  $form['bundle_label'] = array(
    '#type' => 'textfield',
    '#title' => "Bundle (optional)",
    '#description' => "A bundle with the same name as the entity type is created by default, this will override the default",
  );

  $form['bundle'] = array(
    '#type' => 'machine_name',
    '#required' => FALSE,
    '#machine_name' => array(
      'exists' => '_eck_fake_exists',
      'source' => array('bundle_label'),
    ),
  );
  
  $form['#validate'][] = 'eck__entity_type__add_validate';

  $form['submit'] = array(
    '#type' => 'submit',
    '#weight' => 10000,
    '#value' => t('Save'),
  );

  return $form;
}


/**
 * When an entity type is being addes, we need to make sure that
 * its name is unique
 */
function eck__entity_type__add_validate($form, &$state) {
  $info = entity_get_info();
  $entity_types = array_keys($info);
  $entity_type = $state['values']['entity_type'];
  if (in_array($entity_type, $entity_types)) {
    form_set_error('name', t("Entity {$entity_type} already exists"));
  }
}

/**
 * submit handler for adding and entity type
 */
function eck__entity_type__add_submit($form, &$form_state) {

  //This are required so I don't have to do any checks
  $entity_type = $form_state['values']['entity_type'];
  $entity_type_label = $form_state['values']['entity_type_label'];

  //If the table does not exist, then this is a valid entity name, and we can save it.
  if (!db_table_exists("eck_{$entity_type}")) {

    //lets add the type to the table
    if (!empty($form_state['values']['bundle'])) {
      $bundle = $form_state['values']['bundle'];
      if (!empty($form_state['values']['bundle_label'])) {
        $bundle_label = $form_state['values']['bundle_label'];
      }
      else {
        $bundle_label = ucfirst($bundle);
      }
    }
    else {
      $bundle = $entity_type;
      $bundle_label = $entity_type_label;
    }

    db_insert('eck_types')
        ->fields(array(
          'entity' => $entity_type,
          'type' => $bundle,
          'label' => $bundle_label,
        ))
        ->execute();

    db_insert('eck')
        ->fields(array(
          'name' => $entity_type,
          'label' => $entity_type_label,
        ))
        ->execute();

    db_create_table("eck_{$entity_type}", eck__entity_type__schema($entity_type));

    // Clear info caches in order to pick up newly created entities.
    drupal_get_schema(NULL, TRUE);
    entity_info_cache_clear();
    // Rebuild the menu to pick up entity related paths.
    menu_rebuild();

    drupal_set_message(t('Entity type %entity_type has been created.', array('%entity_type' => $entity_type_label)));

    drupal_goto("admin/structure/eck/{$entity_type}");
  }
  else {
    drupal_set_message(t('Database table %name already exists', array('%name' => $entity_type)), 'error');
  }
}



/*function eck__entity_type__delete($form, &$form_state, $name, $label) {

  $form['entity_name'] =
  array(
  '#type' => 'value',
  '#value' => $name,
  );

  $message = t("Are you sure that you want to delete %label", array("%label" => $label));
  $caption = t("This action is irreversible");

  return confirm_form($form, $message, 'admin/eck', $caption, t('Delete'));
  }

  function eck_delete_submit($form, &$form_state) {

  $name = $form_state['values']['entity_name'];

  //delete the bundle
  field_attach_delete_bundle($name, $name);

  //delete the eck database entry
  db_delete('eck')
  ->condition('name', $name)
  ->execute();

  //drop the table
  db_drop_table("eck_{$name}");

  drupal_cron_run();

  //lets rebuild the menu to remove all entity paths
  menu_rebuild();

  $form_state['redirect'] = "admin/eck";
  } */


/**
 * create the default schema for an entity type
 * this function is called by hook_schema in eck.install
 */
function eck__entity_type__schema($entity_type) {
  $schema = array(
    'description' => "The base table for a(n) {$entity_type}.",
    'fields' => array(
      'uuid' => array(
        'type' => 'char',
        'length' => 36,
        'not null' => TRUE,
        'default' => '',
        'description' => 'The Universally Unique Identifier.'
      ),
      'id' => array(
        'description' => "The primary identifier for a(n) {$entity_type}.",
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'type' => array(
        'description' => 'The bundle of the entity',
        'type' => 'varchar',
        'default' => '',
        'length' => 255,
        'not null' => TRUE,
      ),
      'uid' => array(
        'description' => "The {users}.uid that owns this {$entity_type}; initially, this is the user that created it.",
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ),
      'created' => array(
        'description' => "The Unix timestamp when the {$entity_type} was created.",
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ),
      'changed' => array(
        'description' => "The Unix timestamp when the {$entity_type} was most recently saved.",
        'type' => 'int',
        'not null' => TRUE,
        'default' => 0,
      ),
      'state' => array(
         'description' => "Entity state",
         'type' => 'int',
         'unsigned' => TRUE,
         'not null' => TRUE,
         'default' => 0
      )
    ),
    'indexes' => array(
      "{$entity_type}_changed" => array('changed'),
      "{$entity_type}_created" => array('created'),
      'uid' => array('uid'),
    ),
    'foreign keys' =>
    array(
      "{$entity_type}_author" =>
      array(
        'table' => 'users',
        'columns' => array('uid' => 'uid'),
      ),
    ),
    'primary key' => array('id'),
  );


  drupal_alter('eck_schema', $schema, $entity_type);

  return $schema;
}



/**
 * Generate the entity info for a specific entity
 *
 * @param $entity_type (String) the machine name of the entiy type
 */
function eck__entity_type__info($entity_type) {
  $info = array();
  $entity_type_label = eck__get_entity_type_label($entity_type);

  if (!drupal_autoload_class($entity_class = eck_get_class_name($entity_type, 'Entity'))) {
    $entity_class = 'Entity';
  }
  if (!drupal_autoload_class($controller_class = eck_get_class_name($entity_type, 'Controller'))) {
    $controller_class = 'EckController';
  }

  $info[$entity_type] = array(
    'label' => t($entity_type_label),
    'base table' => "eck_{$entity_type}",
    'entity class' => $entity_class,
    'controller class' => $controller_class,
    'module' => 'eck',
    'fieldable' => TRUE,
    'entity keys' => array(
      'id' => 'id',
      'bundle' => 'type'
    ),
    'label callback' => 'eck__entity__label',
    'uri callback' => 'eck__entity__uri',
    // Bundles are defined by the entity types below
    'bundles' => array(),
    // Bundle keys tell the FieldAPI how to extract information from the bundle objects
    'bundle keys' => array(
      'bundle' => 'type'
    ),
    //I guess we need at least one view mode for entity_view_modes (the module) to work
    'view modes' => array(
      'teaser' => array(
          'label' => t('Teaser'),
          'custom settings' => TRUE,
      )
    )
  );

  foreach (eck__get_bundles($entity_type) as $bundle) {
    //dpm($bundle,"Bundle");
    
    $bundle_label = eck__get_bundle_label($entity_type, $bundle);
    //dpm($bundle_label, "Bundle Label");
    $info[$entity_type]['bundles'][$bundle] = array(
      'label' => $bundle_label,
      'admin' => array(
        'path' => "admin/structure/eck/{$entity_type}/{$bundle}",
        'access arguments' => array('administer entities')
      )
    );
  }

  return $info;
}

/**
 * Entity specific implementation of hook_entity_property_info_alter().
 */
function eck__entity_type__property_info(&$info, $entity_type) {
  $properties = &$info['properties'];

  $properties['uid']['label'] = t('User');
  $properties['uid']['type'] = 'user';
  #$properties['uid']['description'] = t('The user this storage belongs to.');

  $properties['created']['label'] = t('Created');
  $properties['created']['type'] = 'date';
  #$properties['created']['description'] = t('The Unix timestamp when the entity has been created.');

  $properties['changed']['label'] = t('Changed');
  $properties['changed']['type'] = 'date';
  #$properties['changed']['description'] = t('The Unix timestamp when the entity was most recently saved.');
}