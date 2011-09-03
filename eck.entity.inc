<?php

/**
 * @file
 * 
 * ENTITY
 * 
 * All the menues, pages, and functionality related to administering
 * entities is in this file
 */

function eck__entity__menu($entity_type, $bundle) {
  $menu = array();

  
  $entity_type_label = eck__get_entity_type_label($entity_type);
  $bundle_label = eck__get_bundle_label($entity_type, $bundle);

  //OVERVIEW Entity
  $menu["admin/structure/eck/{$entity_type}/{$bundle}"] = array(
    'title' => "{$bundle_label} Entities",
    'description' => "View all entites of type {$entity_type_label} with bundle {$bundle_label}",
    'page callback' => "eck__entity__overview",
    'page arguments' => array($entity_type, $bundle),
    'access arguments' => array("administer {$entity_type} {$bundle} entities"),
    'weight' => 0,
    'file' => 'eck.entity.inc'
  );

  $menu["admin/structure/eck/{$entity_type}/{$bundle}/overview"] = array(
    'title' => "Overview",
    'type' => MENU_DEFAULT_LOCAL_TASK,
    'weight' => 0
  );

  //CREATE Entity
  $menu["admin/structure/eck/{$entity_type}/{$bundle}/add"] = array(
    'title' => "Add {$bundle_label} Entity",
    'description' => "Add an entity of type {$entity_type_label} with bundle {$bundle_label}",
    'page callback' => "eck__entity__add",
    'page arguments' => array($entity_type, $bundle),
    'access arguments' => array("add {$entity_type} {$bundle} entities"),
    'type' => MENU_LOCAL_ACTION,
    'weight' => 1,
    'file' => 'eck.entity.inc'
  );

  //READ Entity
  $menu["{$entity_type}/{$bundle}/%"] = array(
  'title' => $entity_type,
  'title callback' => "eck_get_entity_label",
  'title arguments' => array($entity_type, 2),
    'description' => "Is there a description callback?",
    'page callback' => "eck__entity__page",
    'page arguments' => array($entity_type, $bundle, 2),
    'access arguments' => array("view {$entity_type} {$bundle} entities"),
    'weight' => 0,
    'file' => 'eck.entity.inc'
  );
  
  $menu["{$entity_type}/{$bundle}/%/view"] = array(
    'title' => "View",
    'type' => MENU_DEFAULT_LOCAL_TASK,
    'weight' => 0
  );

  //UPDATE Entity
  $menu["{$entity_type}/{$bundle}/%/edit"] = array(
    'title' => "Edit",
    'page callback' => "eck__entity__edit",
    'page arguments' => array($entity_type, $bundle, 2),
    'access arguments' => array("edit {$entity_type} {$bundle} entities"),
    'type' => MENU_LOCAL_TASK,
    'weight' => 1,
    'file' => 'eck.entity.inc'
  );

  //DELETE Entity
  $menu["admin/structure/eck/{$entity_type}/{$bundle}/%/delete"] = array(
    'title' => $entity_type . " - " . $bundle,
    'title callback' => "eck_get_entity_label",
    'title arguments' => array($entity_type, 5),
    'page callback' => "drupal_get_form",
    'page arguments' => array('eck__entity__delete', $entity_type, $bundle, 5),
    'access arguments' => array("delete {$entity_type} {$bundle} entities"),
    'file' => 'eck.entity.inc'
  );

  return $menu;
}

function eck__entity__overview($entity_type, $bundle) {

  
  $entity_type_label = eck__get_entity_type_label($entity_type);
  $bundle_label = eck__get_bundle_label($entity_type, $bundle);

  $table = "eck_{$entity_type}";

  //TODO: Mabye I should use entity field query
  //Get all entity instances of this type
  $results = db_select($table, 't')->fields('t')->condition('type', $bundle, '=')->execute();
  
  $ids = array();
  foreach ($results as $record) {
    $ids[] = $record->id;
  }
  
  $entities = entity_load($entity_type, $ids);
  
  
  $build['table'] = entities_table($entities, TRUE);

  $build['sample_code'] = array(
    '#theme' => 'form_element',
    '#title' => t('Optional entity type class'),
    '#description' => t('You may add this custom class for the bundle. To create a new instance of this class programmatically call: %code', array(
      '%code' => "entity_create('{$entity_type}', array('type' => '{$bundle}'));"
    )),
    '#children' => "<pre>" . check_plain("class " . eck_get_class_name($bundle, 'EntityType') . " extends Entity{\n\n}") . "</pre>"
  );

  return $build;
}



function eck__entity__add($entity_type, $bundle) {
  $entity = eck__entity_create($entity_type, $bundle);
  return drupal_get_form("eck__entity__form", $entity);
}



function eck__entity__view($entity_type, $id) {

  if (is_numeric($id)) {
    $entities = entity_load($entity_type, array($id));
  }

  return $entities[$id]->view();
}



function eck__entity__edit($entity_type, $bundle, $id) {
  if (is_numeric($id)) {
    $entities = entity_load($entity_type, array($id));
    $entity = $entities[$id];
  }

  global $user;

  $entity->uid = $user->uid;
  $entity->changed = time();

  return drupal_get_form("eck__entity__form", $entity);
}

function eck__entity__delete($form, &$form_state, $entity_type, $bundle, $id) {
  $entities = entity_load($entity_type, array($id));

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
         '#value' => "admin/structure/eck/{$entity_type}/{$bundle}",
   );
  

  $message = t("Are you sure that you want to delete %id",
          array("%id" => $id));

  $caption = t("This action cannot be undone.");

  return confirm_form($form, $message, "admin/structure/eck/{$entity_type}", $caption, t('Delete'));
}



function eck__entity__delete_submit($form, &$form_state) {
  $entity = $form_state['values']['entity'];
  $entity_type = $form_state['values']['entity_type'];
  $bundle = $form_state['values']['bundle'];
  $submit_redirect = $form_state['values']['submit_redirect'];

  $entity->delete();

  drupal_cron_run();

  //Ok, lets delete the entity
  $form_state['redirect'] = $submit_redirect;
}



function eck__entity__form($form, $form_state, $entity) {

  $form['entity'] = array(
    '#type' => 'value',
    '#value' => $entity
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#weight' => 10000,
    '#value' => t('Save'),
  );

  field_attach_form($entity->entityType(), $entity, $form, $form_state);

  return $form;
}



function eck__entity__form_submit($form, &$form_state) {
  $entity = $form_state['values']['entity'];

  if (isset($form_state['field'])) {
    foreach (array_keys($form_state['field']) as $field) {
      $entity->{$field} = _field_unset_empty($field, $form_state['values'][$field]);
    }
  }

  $entity->save();

  drupal_set_message(t("Entity {$entity->id} - @entity_label has been saved", array("@entity_label" => entity_label($form['#entity_type'], $entity)) ));
}

function eck__entity__label($entity) {
  $hook_names = array(
  "entity_label", 
  "entity_{$entity->entityType()}_label",
  "entity_{$entity->entityType()}_{$entity->type}_label");
  
  foreach ($hook_names as $hook_name) {
    $new_label = module_invoke_all($hook_name, $entity, $entity->id);
    $empty = empty($new_label);
    if (!$empty) {
      break;
    }
  }

  if (!$empty) {
    return $new_label[0];
  }
  else {
    return $entity->id;
  }
}

function eck__entity__uri($entity) {
  return array('path' => "{$entity->entityType()}/{$entity->type}/{$entity->identifier()}");
}

function eck__entity__page($entity_type, $bundle, $id) {

  $build = array();

  $entity_view = eck__entity__view($entity_type, $id);

  $build["{$entity_type}_{$bundle}_page"] = $entity_view;

  return $build;
}