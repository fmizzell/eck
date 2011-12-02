<?php
module_load_include('inc', 'eck', 'eck.classes');
/**
 *@TODO Make sure that the schema for custom entities is being generated correctly
 * Also we need to clean up entity_type__form_submit
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
 * Passthrough from hook_menu().
 *
 * It creates the menu items related to entity type management.
 */
function eck__entity_type__menu() {
  $menu = array();

  // OVERVIEW Entity Type.
  // View all of the created entity types.
  $menu['admin/structure/entity_type'] = array(
    'title' => 'Entity types',
    'description' => 'A centralized administrative section for entity types',
    'page callback' => 'eck__entity_type__list',
    'access arguments' => array('administer entity types'),
    'file' => 'eck.entity_type.inc'
  );

  // ADD Entity.
  $menu['admin/structure/entity_type/add'] = array(
    'title' => 'Add entity type',
    'description' => 'Add a new entity type',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('eck__entity_type__form'),
    'access arguments' => array('add entity types'),
    'type' => MENU_LOCAL_ACTION,
    'weight' => -1,
    'file' => 'eck.entity_type.inc'
  );

  module_load_include('inc', 'eck', 'eck.bundle');
  // Each entity type can have multiple bundles.
  // Now lets create the menus for the bundle administration of each
  // entity type.
  foreach (EntityType::loadAll() as $entity_type) {
    $menu = array_merge($menu, eck__bundle__menu($entity_type));
  }
  return $menu;
}

/**
 * Callback for the entity_type overview.
 */
function eck__entity_type__list() {
  $header = array(t('Name'), array('data' => t('Operations'), 'colspan' => '1'));
  $rows = array();

  foreach (EntityType::loadAll() as $entity_type) {
    $rows[] = 
    array(
      l(t("{$entity_type->label}"), "admin/structure/entity_type/{$entity_type->name}"),
      l(t("delete"), "admin/structure/entity_type/{$entity_type->name}/delete")
    );
  }
  $build['entity_table'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
  );

  return $build;
}

/**
 * Callback for adding entity types functionality.
 * @param $form
 *  Form array provided by the Form API
 * @param $state
 *  array provided by the Form API
 * @param $entity_type_name
 *   (String) the name of an existing entity type
 */
function eck__entity_type__form($form, &$state, $entity_type_name = NULL) {
  
  dpm(drupal_get_schema());
  
  //If this form is being called on a new entity, create a new entity object, otherwise
  //load the corresponding entity
  if (!isset($entity_type_name)) {
    $entity_type = new EntityType();
  }
  else {
    $entity_type = EntityType::loadByName($entity_type_name);
  }
  
  $form['entity_type'] = array(
    '#type' => 'value',
    '#value' => $entity_type,
  );
  
  $form['entity_type_label'] = array(
    '#type' => 'textfield',
    '#title' => t('Entity Type'),
    '#default_value' => $entity_type->label,
    '#description' => t('A human readable name for the entity type'),
    '#required' => TRUE,
    '#disabled' => !$entity_type->is_new,
  );

  $form['entity_type_name'] = array(
    '#type' => 'machine_name',
    '#default_value' => $entity_type->name,
    '#disabled' => !$entity_type->is_new,
    '#machine_name' => array(
      'exists' => '_eck_fake_exists',
      'source' => array('entity_type_label'),
    ),
  );

  
  $form['#validate'][] = 'eck__entity_type__form_validate';
  

  // Only allow entering the desired bundle name when creating a new entity.
  if ($entity_type->is_new) {
    
    $form['bundle_label'] = array(
      '#type' => 'textfield',
      '#title' => 'Bundle (optional)',
      '#description' => 'A bundle with the same name as the entity type is created by default, this will override the default',
    );

    $form['bundle_name'] = array(
      '#type' => 'machine_name',
      '#required' => FALSE,
      '#machine_name' => array(
        'exists' => '_eck_fake_exists',
        'source' => array('bundle_label'),
      ),
    );
  }
  
  module_load_include('inc', 'eck', 'eck.properties');
  //add the property section of the form
  $form = array_merge($form, eck__property__form($entity_type));
  
  //add the custom property section of the form
  $form = array_merge($form, eck__custom_property__form($entity_type));
  
  $form['submit'] = array(
    '#type' => 'submit',
    '#weight' => 10000,
    '#value' => t('Save'),
  );

  return $form;
  
}

/**
 * When an entity type is being addes, we need to make sure that
 * its name is unique.
 *
 * @param $form
 *  Form array provided by the Form API
 * @param $state
 *  array provided by the Form API
 */
function eck__entity_type__form_validate($form, &$state) {
  $info = entity_get_info();
  $entity_types = array_keys($info);
  $entity_type = $state['values']['entity_type'];
  if (in_array($entity_type, $entity_types)) {
    form_set_error('name', t("Entity {$entity_type} already exists"));
  }
}

/**
 * Submit handler for adding and entity type.
 *
 * @param $form
 *  Form array provided by the Form API
 * @param $state
 *  array provided by the Form API
 */
function eck__entity_type__form_submit($form, &$state) {
  
  dpm($state, "State");
  
  // This are required so I don't have to do any checks.
  $entity_type = $state['values']['entity_type'];
  
  //if the entity type is new set up its name and its label
  //create its table, and add the default bundle to the 
  //eck_bundle table
  if($entity_type->is_new){
    
    //Set up name and label
    $entity_type_name = $state['values']['entity_type_name'];
    $entity_type->name = $entity_type_name;
    
    $entity_type_label = $state['values']['entity_type_label'];
    $entity_type->label = $entity_type_label;
    
    //Add the bundle to the table
    //Process the bundle input from the user
    if (!empty($state['values']['bundle_name'])) {
      $bundle_name = $state['values']['bundle_name'];
      if (!empty($state['values']['bundle_label'])) {
        $bundle_label = $state['values']['bundle_label'];
      }
      else {
        $bundle_label = ucfirst($bundle_name);
      }
    }
    else {
      $bundle_name = $entity_type_name;
      $bundle_label = $entity_type_label;
    }
    
    //Let's set up the object and save it to the db.
    $bundle = new Bundle();
    $bundle->entity_type = $entity_type->name;
    $bundle->name = $bundle_name;
    $bundle->label = $bundle_label;
    $bundle->save();
  }
  
  //lets get the user values for properties
  $properties = $state['values']['properties'];
  
  //custom properties are only relevant on edit, not on new entity types,
  //but lets go ahead and give a default to make it easier to deal with.
  if(array_key_exists('custom_properties', $state['values'])){
    $custom_properties = $state['values']['custom_properties'];
  }else{
    $custom_properties =  array();
  }
  
  if(array_key_exists('add_custom_property', $state['values'])){
    $add_custom_property = array_filter($state['values']['add_custom_property']);
  }else{
    $add_custom_property =  array();
  }
  
  //dpm($properties);
  //dpm($custom_properties);
  //dpm($add_custom_property);
  
  foreach($properties as $name => $add){
    if($add){
      $entity_type->addProperty($name);
    }else{
      $entity_type->removeProperty($name);
    }
  }
  
  //Custom properties are a little different than properties, really the only
  //thing that we need to check for is to see if they have a 0, to delete it
  //because adding a custom property is recorded in the add_custom_property 
  //array, not here
  foreach($custom_properties as $name => $add){
    if(!$add){
      $entity_type->removeProperty($name);
    }
  }
  
  if(!empty($add_custom_property)){
    $entity_type->addCustomProperty
    (
      $add_custom_property['name'],
      $add_custom_property['label'],
      $add_custom_property['type']
    );
  }
  
  $entity_type->save();
  
  //Clear info caches in order to pick up newly created entities.
  drupal_get_schema(NULL, TRUE);
  entity_info_cache_clear();
  menu_rebuild();

  drupal_set_message(t('Entity type %entity_type has been created.', array('%entity_type' => $entity_type->label)));
  //drupal_goto("admin/structure/entity_type");
}
/**
 * Delete the entity type
 * 
 * @param $entity_type 
 *  (String) entity type to be deleted 
 */
function eck__entity_type__delete($entity_type){
  
  module_load_include('inc', 'eck', 'eck.bundle');
  //delete all the bundles from this entity type
  $bundles = Bundle::loadByEntityType($entity_type);
  foreach($bundles as $bundle){
    eck__bundle__delete($entity_type, $bundle);
  }
  
  $entity_type->delete();
 
  drupal_set_message("Entity type '{$entity_type->name}' has been deleted");
  //return "<h1>Deletion Completed</h1> <h3>Entity type '{$entity_type}' has been deleted";
}

function eck__entity_type__delete_form($form, &$state, $entity_type_name){
  $entity_type = entity_type_load($entity_type_name);
  $form['entity_type'] = 
  array(
  '#type' => 'value',
  '#value' => $entity_type
  );
  
  $form['submit_redirect'] =
  array(
    '#type' => 'value',
    '#value' => "admin/structure/entity_type",
  );
  
  $message = t("Are you sure that you want to delete the entity type '{$entity_type->name}'");

  $caption = t("All of the data (entities and bundles) from this entity type 
  will be deleted. This action cannot be undone.");

  return 
  confirm_form($form, $message, 
  "admin/structure/eck", $caption, t('Delete'));
}

function eck__entity_type__delete_form_submit($form, &$state){
  $entity_type = $state['values']['entity_type'];
  $submit_redirect = $state['values']['submit_redirect'];
  
  // Ok, lets delete the entity type
  
  eck__entity_type__delete($entity_type);
  
  $state['redirect'] = $submit_redirect;
}

/**
 * Create the default schema for an entity type.
 *
 * @param $entity_type
 *  (Object) entity type as returned by eck__entity_type__load
 *
 * Passthrough for hook_schema().
 */
function eck__entity_type__schema($entity_type) {
  $schema = array(
    'description' => "The base table for a(n) {$entity_type->name}.",
    'fields' => array(
      'id' => array(
        'description' => "The primary identifier for a(n) {$entity_type->name}.",
        'type' => 'serial',
        'not null' => TRUE,
      ),
      'type' => array(
        'description' => 'The bundle of the entity',
        'type' => 'varchar',
        'default' => '',
        'length' => 255,
        'not null' => TRUE,
      )
    ),
    'primary key' => array('id'),
  );
  
  // Add properties to schema definition.
  module_load_include('inc', 'eck', 'eck.properties');
  eck_set_properties_schema($schema, $entity_type);

  return $schema;
}

/**
 * Generate the entity info for a specific entity
 *
 * @param $entity_type
 *  (Object) as returned by eck__entity_type__load
 */
function eck__entity_type__info($entity_type) {
  //dpm($entity_type, "Entity Info ET");
  module_load_include('inc', 'eck', 'eck.bundle');
  $info = array();
  $entity_type_label = $entity_type->label;
  
  $entity_class = "Entity";
  $controller_class = "EntityAPIController";

  /*if (!drupal_autoload_class($entity_class = eck_get_class_name($entity_type->name, 'Entity'))) {
    $entity_class = 'EckEntity';
  }
  if (!drupal_autoload_class($controller_class = eck_get_class_name($entity_type->name, 'Controller'))) {
    $controller_class = 'EckController';
  }*/

  $info[$entity_type->name] = array(
    'label' => t($entity_type_label),
    'base table' => "eck_{$entity_type->name}",
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
    // Bundles are defined by the entity types below.
    'bundles' => array(),
    // Bundle keys tell the FieldAPI how to extract information from the bundle objects.
    'bundle keys' => array(
      'bundle' => 'type'
    ),
    // I guess we need at least one view mode for entity_view_modes (the module) to work.
    'view modes' => array(
      'teaser' => array(
          'label' => t('Teaser'),
          'custom settings' => TRUE,
      )
    )
  );

  foreach (Bundle::loadByEntityType($entity_type) as $bundle) {
    $bundle_label = $bundle->label;
    $path = "admin/structure/entity_type/{$entity_type->name}/{$bundle->name}";
    $info[$entity_type->name]['bundles'][$bundle->name] = array(
      'label' => $bundle_label,
      'admin' => array(
        'path' => $path,
        'access arguments' => array('administer entities')
      ),
      'crud' =>
      array(
        'add' => 
        array(
          'path' => $path."/add", 
        ),
        'edit' => 
        array(
          'path' => $path."/%/edit",
          'entity_id' => 5
        ),
        'delete' => 
        array(
          'path' => $path."/%/delete",
          'entity_id' => 5
        ),
        'view' => 
        array(
          'path' => "{$entity_type->name}/{$bundle->name}/%",
          'entity_id' => 2
        )
      )
    );
  }
  

  return $info;
}

/**
 * Entity Type specific implementation of property info alter.
 */
/*function eck__entity_type__property_info(&$info, $entity_type_object) {
   $properties = &$info['properties'];
  
  //@TODO Shouldn't we be checking on the properies array of our entity_type_object.. maybe entity api
  //is correctly doing this automatically (CHECK)
  if (isset($properties['uid'])) {
    $properties['uid']['label'] = t('User');
    $properties['uid']['type'] = 'user';
    $properties['uid']['description'] = t('The author of this entity.');
  }
  if (isset($properties['created'])) {
    $properties['created']['label'] = t('Created');
    $properties['created']['type'] = 'date';
    $properties['created']['description'] = t('The Unix timestamp when the entity has been created.');
  }
  if (isset($properties['changed'])) {
    $properties['changed']['label'] = t('Changed');
    $properties['changed']['type'] = 'date';
    $properties['changed']['description'] = t('The Unix timestamp when the entity was most recently saved.');
  }

  // Add custom properties.
  foreach ($entity_type_object->custom_properties as $name => $property) {
    $properties[$name]['label'] = t($property['label']);
    $properties[$name]['type'] = $property['type'];
    //$properties[$name]['description'] = '';
  }
}

function eck__entity_type__custom_property_label($property) {
  $type = $property['type'];
  if (!in_array($type, array('text', 'decimal', 'integer', 'date'))) {
    $type = l($type, "admin/structure/eck/{$type}");
  }
  return $property['label'] . ' (' . t('type: !type', array('!type' => $type)) . ')';
}*/

