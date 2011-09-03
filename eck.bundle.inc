<?php
/**
 * @file
 *
 * BUNDLE
 * 
 * All of the menu, pages, and forms related to bundle 
 * administration are in this file
 */

function eck__bundle__menu($entity_type) {
 
  $entity_type_label = eck__get_entity_type_label($entity_type);

  //Create the menus relavant to types
  $menu = array();

  //OVERVIEW Entity types
  $menu["admin/structure/eck/{$entity_type}"] = array(
    'title' => "{$entity_type_label} Bundles" ,
    'description' => "View all the bundles for '{$entity_type_label}'",
    'page callback' => "eck__bundle__overview",
    'page arguments' => array($entity_type),
    'access arguments' => array("administer {$entity_type} bundles"),
    'weight' => 0,
    'file' => 'eck.bundle.inc'
  );

  $menu["admin/structure/eck/{$entity_type}/add"] = array(
    'title' => "Add a(n) '{$entity_type_label}' Bundle ",
    'description' => "Add a(n) new '{$entity_type_label} Bundle'",
    'page callback' => "drupal_get_form",
    'page arguments' => array('eck__bundle__add', $entity_type),
    'access arguments' => array("add {$entity_type} bundles"),
    'type' => MENU_LOCAL_ACTION,
    'weight' => 1,
    'file' => 'eck.bundle.inc'
  );
  
  module_load_include('inc', 'eck', 'eck.entity');
  foreach (eck__get_bundles($entity_type) as $bundle) {
    $menu = array_merge($menu, eck__entity__menu($entity_type, $bundle));
  }

  return $menu;
}

function eck__bundle__overview($entity_type) {

  $header = array(t('Type'), array('data' => t('Operations'), 'colspan' => '1'));
  $rows = array();

  foreach (eck__get_bundles($entity_type) as $bundle) {
    
    $bundle_label =  eck__get_bundle_label($entity_type, $bundle);
    
    $uri = "admin/structure/eck/{$entity_type}/{$bundle}";
    $row = array(l($bundle_label, url($uri, array('absolute' => TRUE))));
    //$row[] = array('data' => l(t('delete'), "admin/structure/eck/{$name}/{$id}/delete"));
    $rows[] = $row;
  }

  $build['bundle_table'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
  );

  $build['sample_code'] = array(
    '#theme' => 'form_element',
    '#title' => t('Optional entity class'),
    '#description' => t('You may add this custom class the entity. To create a new instance of this class programmatically call: %code', array(
      '%code' => "entity_create('{$entity_type}', array());"
    )),
    '#children' => "<pre>" . check_plain("class " . eck_get_class_name($entity_type, 'EntityType') . " extends Entity{\n\n}") . "</pre>"
  );
  
  return $build;
}

//ADD Entity types
function eck__bundle__add($form, &$form_state, $entity_type) {

  $form['entity_type'] = array(
    '#type' => 'value',
    '#value' => $entity_type,
  );

  $form['bundle_label'] = array(
    '#type' => 'textfield',
    '#title' => "Type",
    '#description' => "A Human readable name for the bundle",
  );

  $form['bundle'] = array(
    '#type' => 'machine_name',
    '#required' => FALSE,
    '#machine_name' => array(
      'exists' => '_eck_fake_exists',
      'source' => array('bundle_label'),
    )
  );

  $form['#validate'][] = 'eck__bundle__add_validate';

  $form['submit'] = array(
    '#type' => 'submit',
    '#weight' => 10000,
    '#value' => t('Save'),
  );

  return $form;
}

function eck__bundle__add_validate($form, &$form_state) {

  $entity_type = $form_state['values']['entity_type'];

  //the type does not have to be unique in the table, but it should be unique to its entity so we will check that here
  foreach (eck__get_bundles($entity_type) as $bundle) {
    if ($bundle == $form_state['values']['bundle']) {
      form_set_error('bundle', t("bundle '{$bundle} already exists for this entity"));
    }
  }
}

function eck__bundle__add_submit($form, &$form_state) {

  $entity_type = $form_state['values']['entity_type'];
  $bundle = $form_state['values']['bundle'];
  $bundle_label = $form_state['values']['bundle_label'];

  db_insert('eck_types')
      ->fields(array(
        'entity' => $entity_type,
        'type' => $bundle,
        'label' => $bundle_label,
      ))
      ->execute();

  drupal_set_message(t('the %bundle for entity type %entity_type has been saved', array('%bundle' => $bundle, '%entity_type' => $entity_type)));

  drupal_get_schema(NULL, TRUE);
  entity_info_cache_clear();
  menu_rebuild();

  drupal_goto("admin/structure/eck/{$entity_type}/{$bundle}");
}