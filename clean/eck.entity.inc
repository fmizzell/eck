<?php
/**
 * @file
 * All the menus, pages, and functionality related to administering entities.
 */

//a few helper function to get data our of the info array
function get_bundle_admin_info($entity_type, $bundle){
  $info = entity_get_info();
  
  return $info[$entity_type]['bundles'][$bundle]['admin'];
}
function get_bundle_crud_info($entity_type_name, $bundle_name){
  $info = entity_get_info();
  return $info[$entity_type_name]['bundles'][$bundle_name]['crud'];
}


/**
 * Get the entities view
 *
 * @param $entity_type
 *  (String) entity type
 * @param $id
 *  (mixed) The entity id or the entity object itself
 *  Normally you wouldn't need to call this function if you already have the
 *  loaded entity but there are some workflows where an object is passed.
 *  So this function handles that case as well.
 */
function eck__entity__build($entity_type, $bundle, $entity) {
  
  if (!$entity) {
    drupal_not_found();
    exit();
  }
  else {
    if ($entity->type == $bundle->name) {
      return $entity->view();
    }
    else {
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
function eck__entity__edit($entity_type_name, $bundle_name, $entity) {
  return drupal_get_form("eck__entity__form_edit_{$entity_type_name}_{$bundle_name}", $entity);
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
 * @param $entity
 *  (object) entity object to be deleted.
 */
function eck__entity__delete($entity_type_name, $bundle_name, $entity) {
  $entity_type = entity_type_load($entity_type_name);
  $bundle = bundle_load($entity_type_name, $bundle_name);
  return drupal_get_form('eck__entity__delete_form', $entity_type, $bundle, $entity);
  
}

/**
 * Delete form
 */
function eck__entity__delete_form($form, &$form_state, $entity_type, $bundle, $entity){
  $path = eck__entity_type__path();

  $form['entity'] =
      array(
        '#type' => 'value',
        '#value' => $entity,
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
    '#value' => "{$path}/{$entity_type->name}/{$bundle->name}",
  );

  $message = t("Are you sure that you want to delete %id",
          array("%id" => $entity->id));

  $caption = t("This action cannot be undone.");

  return confirm_form($form, $message, "{$path}/{$entity_type->name}", $caption, t('Delete'));
  
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
 * Creates a renderable array to show an entity
 *
 * @param $entity_type
 *  (String) entity type
 * @param $bundle
 *  (String) Bundle
 * @param $id
 *  (mixed) ID or Entity Object being viewed
 */
function eck__entity__view($entity_type_name, $bundle_name, $entity) {

  $entity_type = entity_type_load($entity_type_name);
  $properties = $entity_type->properties;
  $bundle = bundle_load($entity_type_name, $bundle_name);
  
  $build = array();
  $entity_view = eck__entity__build($entity_type, $bundle, $entity);
  $property_view = array();
  
  $formatters = eck_property_behavior_invoke_plugin($entity_type, 'default_formatter', 
    array('entity' => $entity));
    
  foreach($formatters as $property => $formatter){
    $property_view[$property] = $formatter;
  }
  
  $entity_view[$entity->entityType()][$entity->id] = array_merge($property_view, $entity_view[$entity->entityType()][$entity->id]);
  
  eck_property_behavior_invoke_plugin($entity_type, 'entity_view',
    array('entity' => $entity));
  
  $build["{$entity_type->name}_{$bundle->name}_page"] = $entity_view;

  return $build;
}
