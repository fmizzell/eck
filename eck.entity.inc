<?php
/**
 * @file
 * All the menus, pages, and functionality related to administering entities.
 */

/**
 * This function creates the menu items related to entity administration
 * @param $entity_type
 *  (String) the entity type
 * @param $bundle
 *  (String) the bundle of the entity
 *
 * this function is called from hook_menu()
 * @see eck_menu()
 */
function eck__entity__menu($entity_type, $bundle) {
  /*dpm("Entity Menu");
  dpm($entity_type, "Entity Type");
  dpm($bundle, "Bundle");*/
  $path = eck__entity_type__path();
  $menu = array();
  
  // DELETE Bundle
  $menu["{$path}/{$entity_type->name}/{$bundle->name}/delete"] = array(
    'title' => "Delete",
    'page callback' => "drupal_get_form",//"eck__bundle__delete",
    'page arguments' => array('eck__bundle__delete_form', 3, 4),
    'access arguments' => array("delete {$entity_type->name} {$bundle->name} bundle"),
    'file' => 'eck.bundle.inc',
    'type' => MENU_LOCAL_TASK
  );
  
  //The entity autocomplete module does a great job at providing generalize
  //autocomplete function that will work for any entity that declares the label
  //property in the entity_info array, but there is not a general solution to
  //have autocomplete functionality from a field. So I will fill that gap by
  //declaring a field_autocomplete for all eck entities
  /*$menu["admin/structure/eck/{$entity_type->name}/{$bundle->name}/field-autocomplete"] = array(
    'title' => "Field Autocomplete for {$entity_type->name}:{$bundle->name}",
    'page callback' => "eck__bundle__field_autocomplete",
    'page arguments' => array($entity_type, $bundle),
    'access arguments' => array("autocomplete {$entity_type->name} {$bundle->name} bundle"),
    'file' => 'eck.bundle.inc'
  );*/
  
  $admin_info = get_bundle_admin_info($entity_type->name, $bundle->name);
  
  // OVERVIEW Entity
  $menu[$admin_info['path']] = array(
    'title' => "{$bundle->label}",
    'description' => "View all entites of type {$entity_type->label} with bundle {$bundle->label}",
    'page callback' => "eck__entity__list",
    'page arguments' => array($entity_type->name, $bundle->name),
    'access arguments' => array("administer {$entity_type->name} {$bundle->name} entities"),
    'weight' => 0,
    'file' => 'eck.entity.inc'
  );

  $menu[$admin_info['path']."/list"] = array(
    'title' => "Entity List",
    'type' => MENU_DEFAULT_LOCAL_TASK,
    'weight' => 100
  );
  
  $crud_info = get_bundle_crud_info($entity_type->name, $bundle->name);
  
  foreach($crud_info as $action => $info){
      
    $action_label = ucfirst($action);
    $args = array();
    
    if(array_key_exists('entity_id', $info)){
      $args[] = $info['entity_id'];
    }
    
    $args = array_merge(array($entity_type->name, $bundle->name), $args);
    
    $menu[$info['path']] = array(
      'title' => "{$action_label} Entity",
      'description' => "{$action_label} an entity of type {$entity_type->label} with bundle {$bundle->label}",
      'page callback' => "eck__entity__{$action}",
      'page arguments' => $args,
      'access arguments' => array("{$action} {$entity_type->name} {$bundle->name} entities"),
      'file' => 'eck.entity.inc'
    );
    
    //I think it would be useful to have the edit, delete, and list tabs at the view also
    //But lets leave this out for right now
    /*if($action == 'view'){
      
      $menu[$info['path']."/view"] = array(
        'title' => "View",
        'type' => MENU_DEFAULT_LOCAL_TASK
      );
      
      $menu[$info['path']."/list"] = array(
        'title' => "List",
        'description' => "View all entites of type {$entity_type->label} with bundle {$bundle->label}",
        'page callback' => "eck__entity__list",
        'page arguments' => array($entity_type->name, $bundle->name),
        'access arguments' => array("administer {$entity_type->name} {$bundle->name} entities"),
        'weight' => 0,
        'file' => 'eck.entity.inc',
        'type' => MENU_LOCAL_TASK
      );
      
      
      foreach($crud_info as $a => $i){
        if($a != 'view' && $a != 'add'){
          $al = ucfirst($a);
          
          $view_path = $info['path']."/{$a}";
          //dpm($view_path, "Path");
          
          $menu[$view_path] = array(
            'title' => "{$al}",
            'description' => "{$action_label} an entity of type {$entity_type->label} with bundle {$bundle->label}",
            'page callback' => "eck__entity__{$a}",
            'page arguments' => $args,
            'access arguments' => array("{$action} {$entity_type->name} {$bundle->name} entities"),
            'file' => 'eck.entity.inc',
            'type' => MENU_LOCAL_TASK
          );
        }
      }
    }*/
    //Holy Crap What a mess @todo clean up ^^
  }
  
  return $menu;
}

//a few helper function to get data our of the info array
function get_bundle_admin_info($entity_type, $bundle){
  $info = entity_get_info();
  //dpm($info, "Info");
  
  return $info[$entity_type]['bundles'][$bundle]['admin'];
}
function get_bundle_crud_info($entity_type_name, $bundle_name){
  //dpm($entity_type_name);
  //dpm($bundle_name);
  $info = entity_get_info();
  return $info[$entity_type_name]['bundles'][$bundle_name]['crud'];
}

/**
 * This is the callback function for the entity overview page. This page shows all
 * of the entities created of a given type and bundle
 * @param $entity_type
 *  (String) entity type
 * @param $bundle
 *  (String) Bundle
 */
function eck__entity__list($entity_type_name, $bundle_name) {
  
  $entity_type = entity_type_load($entity_type_name);
  $bundle = bundle_load($entity_type_name, $bundle_name);
  
  $info['entity_type'] = $entity_type->name;
  $info['bundle'] = $bundle->name;

  $table = "eck_{$entity_type->name}";

  // @todo Mabye I should use entity field query
  // Get all entity instances of this type
  $query = new EntityFieldQuery();
  $query
  ->entityCondition('entity_type', $entity_type->name, '=')
  ->entityCondition('bundle', $bundle->name, '=');
  
  drupal_alter('entity_overview_query', $query, $info);
  unset($info['entity_type']);
  drupal_alter("entity_{$entity_type->name}_overview_query", $query, $info);
  drupal_alter("entity_{$entity_type->name}_{$bundle->name}_overview_query", $query);
  
  $results = $query->execute();
  if(!empty($results)){
    $entities = entity_load($entity_type->name, array_keys($results[$entity_type->name]));
  }else{
    $entities = array();
  }
  
  //Because of the flexible paths capabilities, we are not guaranteed to see a local action for the add here,
  //so lets add a link ourselves until we figure out whether there is a better solution
  $crud_info = get_bundle_crud_info($entity_type->name, $bundle->name);
  $build['add'] = 
  /*array(
    '#theme'=> 'menu_local_action', 
    '#link'=>
      array('title' => 'Add entity', 'href' => $crud_info['add']['path']));*/
      
  array('#markup' => "<ul class = 'action-links'><li>".l(t('Add Entity'), $crud_info['add']['path'])."</li></ul>");

  $build['table'] = entity_table($entities, TRUE);

  /*$build['sample_code'] = array(
    '#theme' => 'form_element',
    '#title' => t('Optional entity type class'),
    '#description' => t('You may add this custom class for the bundle. To create a new instance of this class programmatically call: %code', array(
      '%code' => "entity_create('{$entity_type->name}', array('type' => '{$bundle->name}'));"
    )),
    '#children' => "<pre>" . check_plain("class " . eck_get_class_name($bundle->name, 'EntityType') . " extends Entity{\n\n}") . "</pre>"
  );*/

  return $build;
}

/**
 * Call back for the local action add (It adds a new entity)
 *
 * @param $entity_type
 *  (String) entity type
 * @param $bundle
 *  (String) Bundle
 */
function eck__entity__add($entity_type_name, $bundle_name) {
  $entity_type = entity_type_load($entity_type_name);
  $bundle = bundle_load($entity_type_name, $bundle_name);
  
  $entity = entity_create($entity_type->name, array('type' => $bundle->name));
  //dpm($entity);
  //dpm(drupal_get_schema(), "Schema");
  return drupal_get_form("eck__entity__form", $entity);
}

/**
 * Get the entities view
 *
 * @param $entity_type
 *  (String) entity type
 * @param $id
 *  (int) The entities id
 */
function eck__entity__build($entity_type, $bundle, $id) {
  if (is_numeric($id)) {
    
    $entities = entity_load($entity_type->name, array($id));
    if(array_key_exists($id, $entities)){
     $entity = $entities[$id];
    }else{
      $entity = NULL;
    }
  }else{
    drupal_not_found();
    exit();
  }
  
  if(!$entity){
    drupal_not_found();
    exit();
  }else{
    if($entity->type == $bundle->name){
      return $entity->view();
    }else{
      drupal_not_found();
      exit();
    }

  }
}

/**
 * Callback function for an entities edit page
 *
 * @param $entity_type
 *  (String) entity type
 * @param $bundle
 *  (String) Bundle
 * @param $id
 *  (int) the Id of the entity to be edited
 */
function eck__entity__edit($entity_type_name, $bundle_name, $id) {
  if (is_numeric($id)) {
    $entities = entity_load($entity_type_name, array($id));
    $entity = $entities[$id];
  }

  global $user;

  return drupal_get_form("eck__entity__form", $entity);
}

/**
 * Callback function for the delete functionality
 *
 * @param $form
 *  Form array provided by the Form API
 * @param $form_state
 *  array provided by the Form API
 * @param $entity_type
 *  (String) entity type
 * @param $bundle
 *  (String) Bundle
 * @param $id
 *  (int) the Id of the entity to be deleted
 */
function eck__entity__delete($entity_type_name, $bundle_name, $id) {
  $entity_type = entity_type_load($entity_type_name);
  $bundle = bundle_load($entity_type_name, $bundle_name);
  return drupal_get_form('eck__entity__delete_form', $entity_type, $bundle, $id);
  
}

function eck__entity__delete_form($form, &$form_state, $entity_type, $bundle, $id){
  $entities = entity_load($entity_type->name, array($id));

  $form['entity'] =
      array(
        '#type' => 'value',
        '#value' => $entities[$id],
  );

  $form['entity_type'] =
      array(
        '#type' => 'value',
        '#value' => $entity_type,
  );

  $form['bundle'] =
      array(
        '#type' => 'value',
        '#value' => $bundle,
  );

  $form['submit_redirect'] =
  array(
    '#type' => 'value',
    '#value' => "admin/structure/entity_type/{$entity_type->name}/{$bundle->name}",
  );

  $message = t("Are you sure that you want to delete %id",
          array("%id" => $id));

  $caption = t("This action cannot be undone.");

  return confirm_form($form, $message, "admin/structure/eck/{$entity_type->name}", $caption, t('Delete'));
  
}

/**
 * Sumbmit function for the delete functionality
 *
 * @param $form
 *  Form array provided by the Form API
 * @param $form_state
 *  array provided by the Form API
 */
function eck__entity__delete_form_submit($form, &$form_state) {
  $entity = $form_state['values']['entity'];
  $entity_type = $form_state['values']['entity_type'];
  $bundle = $form_state['values']['bundle'];
  $submit_redirect = $form_state['values']['submit_redirect'];

  $entity->delete();

  drupal_cron_run();

  // Ok, lets delete the entity
  $form_state['redirect'] = $submit_redirect;
}

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
function eck__entity__form($form, $form_state, $entity) {
  $form['entity'] = array(
    '#type' => 'value',
    '#value' => $entity
  );
  
  // Property Widget Handling 
  $property_info = entity_get_property_info($entity->entityType());
  $properties = array();
  foreach($property_info['properties'] as $pname => $pi){
    if(array_key_exists('widget', $pi)){
      $widget_callback = $pi['widget'];
      $widget = $widget_callback($entity);
      $properties[$pname] = $widget_callback;
      $form[$pname] = $widget_callback($entity);
    }
  }
  
  $form['submit'] = array(
    '#type' => 'submit',
    '#weight' => 10000,
    '#value' => t('Save'),
  );

  field_attach_form($entity->entityType(), $entity, $form, $form_state);

  return $form;
}

/**
 * Submit function for entity form
 *
 * @param $form
 *  Form array provided by the Form API
 * @param $form_state
 *  array provided by the Form API
 */
function eck__entity__form_submit($form, &$state) {
  $entity = $state['values']['entity'];
  
  field_attach_form_validate($entity->entityType(), $entity, $form, $state);
  field_attach_submit($entity->entityType(), $entity, $form, $state);
  
  $entity_type = $entity->entityType();
  $entity_type = EntityType::loadByName($entity_type);
  $properties = $entity_type->properties;
  
  foreach($properties as $property => $info){
    //If there is a behavior associated with this property we need to call the appropiate hooks
    if(array_key_exists('behavior', $info) && !empty($info['behavior'])){
      $behavior = $info['behavior'];
     
      $plugin = ctools_get_plugins('eck', 'property_behavior', $behavior);
      _eck_apply_plugin_function('entity_save', $plugin, $entity, $property, $state);
      if(isset($entity->is_new) && $entity->is_new){
        _eck_apply_plugin_function('entity_insert', $plugin, $entity, $property, $state);
      }else{
         _eck_apply_plugin_function('entity_update', $plugin, $entity, $property, $state);
      }
    }
  }
  
  $entity->save();

  drupal_set_message(t("Entity {$entity->id} - @entity_label has been saved", array("@entity_label" => entity_label($form['#entity_type'], $entity)) ));
}

function _eck_apply_plugin_function($function_name, $plugin, $entity, $property, $state){
  
  //Now we get the funtions from the plugin to handle save, insert and update
  $function = ctools_plugin_get_function($plugin, $function_name);
  
  if($function){
    $value = _eck_form_property_value($state, $property);
    $function($entity, $property, $value);
  }
}

function _eck_form_property_value($state, $property){
  if(array_key_exists($property, $state['values'])){
    return $state['values'][$property];
  }
  return NULL;
}


/**
 * Creates a renderable array to show an entity
 *
 * @param $entity_type
 *  (String) entity type
 * @param $bundle
 *  (String) Bundle
 * @param $id
 *  (int) the Id of the entity to be deleted
 */
function eck__entity__view($entity_type_name, $bundle_name, $id) {
  $entity_type = entity_type_load($entity_type_name);
  $bundle = bundle_load($entity_type_name, $bundle_name);
  
  $build = array();
  $entity_view = eck__entity__build($entity_type, $bundle, $id);
  $build["{$entity_type->name}_{$bundle->name}_page"] = $entity_view;

  return $build;
}
