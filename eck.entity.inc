<?php
/**
 * @file
 * All the menus, pages, and functionality related to administering entities.
 */

/**
 * Entity related menu items.
 */
function eck__entity__menu($entity_type, $bundle) {
  $path = eck__entity_type__path();
  $menu = array();

  // DELETE Bundle.
  $menu["{$path}/{$entity_type->name}/{$bundle->name}/delete"] = array(
    'title' => "Delete",
    'page callback' => "drupal_get_form",
    'page arguments' => array('eck__bundle__delete_form', 3, 4),
    'access callback' => 'eck_access',
    'access arguments' => array('delete', 'bundle', $entity_type),
    'file' => 'eck.bundle.inc',
    'type' => MENU_LOCAL_TASK,
  );

  // EDIT Bundle.
  $menu["{$path}/{$entity_type->name}/{$bundle->name}/edit"] = array(
    'title' => 'Edit',
    'page callback' => 'drupal_get_form',
    'page arguments' => array(
      'eck__bundle__edit_form',
      $entity_type->name,
      $bundle->name,
    ),
    'access callback' => 'eck_access',
    'access arguments' => array('update', 'bundle', $bundle),
    'file' => 'eck.bundle.inc',
    'type' => MENU_LOCAL_TASK,
  );

  // Managing a bundle's properties as extra fields.
  $menu["{$path}/{$entity_type->name}/{$bundle->name}/properties/%"] = array(
    'title arguments' => array(6),
    'page callback' => 'drupal_get_form',
    'page arguments' => array(
      'eck__manage_extra_field_form',
      $entity_type->name,
      $bundle->name,
      6,
    ),
    'access callback' => 'eck_access',
    'access arguments' => array('update', 'bundle', $bundle),
    'file' => 'eck.bundle.inc',
  );

  $menu["{$path}/{$entity_type->name}/{$bundle->name}/properties/%/edit"] = array(
    'title' => 'Edit',
    'type' => MENU_DEFAULT_LOCAL_TASK,
    'weight' => 0,
  );

  $menu["{$path}/{$entity_type->name}/{$bundle->name}/properties/%/widget-type"] = array(
    'title' => 'Widget Type',
    'page callback' => 'drupal_get_form',
    'page arguments' => array(
      'eck__extra_field_widget_form',
      $entity_type->name,
      $bundle->name,
      6,
    ),
    'access callback' => 'eck_access',
    'access arguments' => array('update', 'bundle', $bundle),
    'file' => 'eck.bundle.inc',
    'type' => MENU_LOCAL_TASK,
    'weight' => 5,
  );

  $menu["{$path}/{$entity_type->name}/{$bundle->name}/properties/%/remove"] = array(
    'title' => 'Remove',
    'page callback' => 'drupal_get_form',
    'page arguments' => array(
      'eck__remove_extra_field_form',
      $entity_type->name,
      $bundle->name,
      6,
    ),
    'access callback' => 'eck_access',
    'access arguments' => array('update', 'bundle', $bundle),
    'file' => 'eck.bundle.inc',
    'type' => MENU_LOCAL_TASK,
    'weight' => 10,
  );

  $admin_info = get_bundle_admin_info($entity_type->name, $bundle->name);

  // OVERVIEW Entity.
  $menu[$admin_info['path']] = array(
    'title' => "{$bundle->label}",
    'description' => "View all entites of type {$entity_type->label} with bundle {$bundle->label}",
    'page callback' => "eck__entity__list",
    'page arguments' => array($entity_type->name, $bundle->name),
    'access callback' => 'eck_access',
    'access arguments' => array('list', 'entity', $bundle),
    'weight' => 0,
    'file' => 'eck.entity.inc',
  );

  $menu[$admin_info['path'] . "/list"] = array(
    'title' => "Entity List",
    'type' => MENU_DEFAULT_LOCAL_TASK,
    'weight' => 100,
  );

  $crud_info = get_bundle_crud_info($entity_type->name, $bundle->name);

  foreach ($crud_info as $action => $info) {

    switch ($action) {
      case 'add':
        $perm_op = 'create';
        break;

      case 'edit':
        $perm_op = 'update';
        break;

      default:
        $perm_op = $action;
        break;
    }

    $perm_args = array($perm_op, 'entity');

    $action_label = ucfirst($action);
    $args = array();

    if (array_key_exists('entity_id', $info)) {
      $args[] = $info['entity_id'];
      $perm_args[] = $info['entity_id'];
    }
    else {
      $perm_args[] = $bundle;
    }

    $args = array_merge(array($entity_type->name, $bundle->name), $args);

    $menu[$info['path']] = array(
      'title' => "{$action_label} {$bundle->label}",
      'description' => "{$action_label} an entity of type {$entity_type->label} with bundle {$bundle->label}",
      'page callback' => "eck__entity__{$action}",
      'page arguments' => $args,
      'load arguments' => array($entity_type->name),
      'access callback' => 'eck_access',
      'access arguments' => $perm_args,
      'file' => 'eck.entity.inc',
    );

    if ($action == 'view') {

      $menu[$info['path'] . "/view"] = array(
        'title' => "View",
        'type' => MENU_DEFAULT_LOCAL_TASK,
        'weight' => 0,
      );
    }
  }

  return $menu;
}

/**
 * Get admin bundle information from the entity info array.
 */
function get_bundle_admin_info($entity_type, $bundle) {
  $info = entity_get_info();

  return $info[$entity_type]['bundles'][$bundle]['admin'];
}

/**
 * Get bundle crud information from the entity info array.
 */
function get_bundle_crud_info($entity_type_name, $bundle_name) {
  $info = entity_get_info();
  return $info[$entity_type_name]['bundles'][$bundle_name]['crud'];
}

/**
 * Entity overview page callback.
 */
function eck__entity__list($entity_type_name, $bundle_name) {
  $entity_type = entity_type_load($entity_type_name);
  $bundle = bundle_load($entity_type_name, $bundle_name);

  $info['entity_type'] = $entity_type->name;
  $info['bundle'] = $bundle->name;

  $table = "eck_{$entity_type->name}";

  // Get all entity instances of this type.
  $query = new EntityFieldQuery();
  $query
  ->entityCondition('entity_type', $entity_type->name, '=')
  ->entityCondition('bundle', $bundle->name, '=')
  ->pager(20);

  drupal_alter('entity_overview_query', $query, $info);
  unset($info['entity_type']);
  drupal_alter("entity_{$entity_type->name}_overview_query", $query, $info);
  drupal_alter("entity_{$entity_type->name}_{$bundle->name}_overview_query", $query);

  $results = $query->execute();
  if (!empty($results)) {
    $entities = entity_load($entity_type->name, array_keys($results[$entity_type->name]));
  }
  else {
    $entities = array();
  }

  $destination = drupal_get_destination();

  // Because of the flexible paths capabilities, we are not guaranteed to see a
  // local action for the add here, so lets add a link ourselves until we figure
  // out whether there is a better solution.
  $crud_info = get_bundle_crud_info($entity_type->name, $bundle->name);

  // Check that the user has permissions to add an entity:
  if (eck_access('create', 'entity') || eck_access('create', 'entity', $bundle)) {
    $build['actions'] = array(
      '#theme' => 'links',
      '#links' => array(
        array(
          'title' => t("Add") . " $bundle->label",
          'href' => $crud_info['add']['path'],
          'query' => $destination,
        ),
      ),
      '#attributes' => array(
        'class' => array('action-links'),
      ),
    );
  }

  $build['table'] = entity_table($entities, TRUE);
  $build['pager'] = array('#theme' => 'pager');

  return $build;
}

/**
 * Call back for the local action add (It adds a new entity).
 *
 * @param string $entity_type_name
 *   Entity type.
 * @param string $bundle_name
 *   Bundle.
 */
function eck__entity__add($entity_type_name, $bundle_name) {
  $entity_type = entity_type_load($entity_type_name);
  $bundle = bundle_load($entity_type_name, $bundle_name);

  $entity = entity_create($entity_type->name, array('type' => $bundle->name));
  return drupal_get_form("eck__entity__form_add_{$entity_type_name}_{$bundle_name}", $entity);
}

/**
 * Get the entities view.
 */
function eck__entity__build($entity_type, $bundle, $id, $view_mode = "full") {

  if (is_object($id) && $id->entityType() == $entity_type->name && $id->bundle() == $bundle->name) {
    return $id->view($view_mode);
  }
  elseif (is_numeric($id)) {
    $entity = entity_load_single($entity_type->name, $id);
    return $entity->view($view_mode);
  }

  drupal_not_found();
  drupal_exit();
}

/**
 * Callback function for an entities edit page.
 *
 * @param string $entity_type_name
 *   Entity type.
 * @param string $bundle_name
 *   Bundle.
 * @param int $id
 *   the Id of the entity to be edited.
 */
function eck__entity__edit($entity_type_name, $bundle_name, $id) {
  if (is_numeric($id)) {
    $entity = entity_load_single($entity_type_name, $id);
  }

  elseif (is_object($id) and $id->entityType() === $entity_type_name) {
    $entity = $id;
  }

  return drupal_get_form("eck__entity__form_edit_{$entity_type_name}_{$bundle_name}", $entity);
}

/**
 * Callback function for the delete functionality.
 */
function eck__entity__delete($entity_type_name, $bundle_name, $entity) {
  $entity_type = entity_type_load($entity_type_name);
  $bundle = bundle_load($entity_type_name, $bundle_name);
  return drupal_get_form('eck__entity__delete_form', $entity_type, $bundle, $entity);
}

/**
 * Entity delete form.
 */
function eck__entity__delete_form($form, &$form_state, $entity_type, $bundle, $entity) {
  $path = eck__entity_type__path();

  $form['entity'] = array(
    '#type' => 'value',
    '#value' => $entity,
  );

  $form['entity_type'] = array(
    '#type' => 'value',
    '#value' => $entity_type,
  );

  $form['bundle'] = array(
    '#type' => 'value',
    '#value' => $bundle,
  );

  $form['submit_redirect'] = array(
    '#type' => 'value',
    '#value' => "{$path}/{$entity_type->name}/{$bundle->name}",
  );

  $message = t("Are you sure that you want to delete %id",
    array("%id" => $entity->id));

  $caption = t("This action cannot be undone.");

  return confirm_form($form, $message, "{$path}/{$entity_type->name}", $caption, t('Delete'));

}

/**
 * Sumbmit function for the delete functionality.
 */
function eck__entity__delete_form_submit($form, &$form_state) {
  $entity = $form_state['values']['entity'];
  $entity_type = $form_state['values']['entity_type'];
  $bundle = $form_state['values']['bundle'];
  $submit_redirect = $form_state['values']['submit_redirect'];

  $entity->delete();

  // Ok, lets delete the entity.
  $form_state['redirect'] = $submit_redirect;
}

/**
 * Sets up an entities form.
 */
function eck__entity__form($form, &$form_state, $entity) {
  $form['entity'] = array(
    '#type' => 'value',
    '#value' => $entity,
  );

  // Property Widget Handling.
  $entity_type = entity_type_load($entity->entityType());
  $bundle = bundle_load($entity_type->name, $entity->type);
  $property_info = entity_get_property_info($entity->entityType());
  $language = LANGUAGE_NONE;
  if (function_exists("entity_language")) {
    $language = entity_language($entity_type->name, $entity);
  }

  $properties = array();

  foreach ($entity_type->properties as $property_name => $info) {
    if (empty($bundle->config['extra_fields'][$property_name]['form'])) {
      continue;
    }

    $bundle_property_config = $bundle->config['extra_fields'][$property_name]['form'];
    $widget_type = eck_property_info_widget_types($bundle_property_config['widget']['type']);

    // Get the default value for this property.
    $value = NULL;
    if (isset($entity->{$property_name})) {
      $value = $entity->{$property_name};
    }
    elseif (isset($bundle_property_config['default_value_function'])) {
      $value = $bundle_property_config['default_value_function']($entity_type, $bundle, $entity);
    }
    elseif ($bundle_property_config['default_value']) {
      $value = $bundle_property_config['default_value'];
    }

    // Include external module file dependency if one is required.
    if (function_exists('drupal_get_path') && $widget_type['file']) {
      form_load_include($form_state, $widget_type['file type'], $widget_type['module'], $widget_type['file']);
    }

    $function = $widget_type['module'] . '_eck_property_widget_form';
    if (function_exists($function)) {
      $element = array(
        '#entity' => $entity,
        '#entity_type' => $entity_type,
        '#bundle' => $bundle,
        '#property_name' => $property_name,
        '#widget_type' => $widget_type,
        '#language' => LANGUAGE_NONE,
        '#title' => check_plain($bundle_property_config['label']),
        '#description' => field_filter_xss($bundle_property_config['description']),
        '#required' => (!empty($bundle_property_config['required'])),
      );
      // Call the widget's form hook and load the widget form element.
      if ($element = $function($form, $form_state, $property_name, $bundle_property_config, $language, $value, $element)) {
        // Allow modules to alter the property widget form element.
        $context = array(
          'form' => $form,
          'property_name' => $property_name,
          'bundle_property_config' => $bundle->config['extra_fields'][$property_name],
          'langcode' => LANGUAGE_NONE,
          'value' => $value,
        );
        drupal_alter(array('eck_property_widget_form', 'eck_property_widget_' . $bundle_property_config['widget']['type'] . '_form'), $element, $form_state, $context);
      }
      $properties[$property_name] = $element;
    }
  }

  // Check the property behaviors for widgets too.
  // @todo Can probably integrate this with the widget hooks and do away
  // with this.
  $vars = array('entity' => $entity);
  $vars += $property_info;

  // Add the property forms to the entity form.
  if (!empty($properties)) {
    $form += $properties;
  }

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#weight' => 10000,
    '#value' => t('Save'),
  );

  field_attach_form($entity->entityType(), $entity, $form, $form_state);

  return $form;
}

/**
 * Validation function for entity form for validating the fields.
 */
function eck__entity__form_validate($form, &$state) {
  $entity = $state['values']['entity'];
  field_attach_form_validate($entity->entityType(), $entity, $form, $state);

  // Lets validate our properties by trying to set them.
  $entity_type_name = $entity->entityType();
  $entity_type = EntityType::loadByName($entity_type_name);
  $properties = $entity_type->properties;

  // If we find a value set for a property lets just set it.
  foreach ($properties as $property => $info) {
    $form_value = _eck_form_property_value($state, $property);

    if (isset($form_value)) {

      // @todo This should be a widget hook not a behavior function.
      $vars = array('data' => $form_value);
      $data = eck_property_behavior_invoke_plugin($entity_type, 'pre_set', $vars);

      if (array_key_exists($property, $data)) {
        $form_value = $data[$property];
      }

      try{
        $entity->{$property} = $form_value;
      }
      catch(Exception $e) {
        // If there was an exception lets set up the proper for error.
        form_set_error($property, "Invalid property value {$form_value}, value should be of type {$info['type']}");
      }
    }
  }
}

/**
 * Submit function for entity form.
 */
function eck__entity__form_submit($form, &$state) {
  $entity = $state['values']['entity'];

  field_attach_submit($entity->entityType(), $entity, $form, $state);

  $entity_type = EntityType::loadByName($entity->entityType());

  // Let the behaviors modify the entity.
  // @todo Why do we need to pass form information to the save behavior.
  // This is related to eck_revisions. Is there a danger that the current
  // eck_revision logic will not apply when entities are manipulated from
  // code and not the UI?
  eck_property_behavior_invoke_plugin($entity_type, 'entity_save',
    array('entity' => $entity, 'form' => $form, 'form_state' => $state));

  $entity->save();

  drupal_set_message(t("Entity @entity_id - @entity_label has been saved",
    array(
      "@entity_id" => $entity->id,
      "@entity_label" => entity_label($form['#entity_type'], $entity),
    )));

  $uri = entity_uri($entity->entityType(), $entity);
  $state['redirect'] = $uri['path'];
}

/**
 * Creates a renderable array to show an entity.
 *
 * @param string $entity_type_name
 *   Entity type.
 * @param string $bundle_name
 *   Bundle.
 * @param mixed $id
 *   ID or Entity Object being viewed.
 */
function eck__entity__view($entity_type_name, $bundle_name, $id) {
  if (is_object($id) && $id->bundle() == $bundle_name) {
    $entity = $id;
  }
  // If the bundle does not match, we are trying to view and entity of the
  // right type but incorrect bundle.
  elseif (is_object($id) && $id->bundle() != $bundle_name) {
    drupal_not_found();
    exit();
  }
  else {
    $entity = entity_load_single($entity_type_name, $id);
  }

  return entity_view($entity_type_name, array($entity));
}

/**
 * Entity view callback.
 */
function eck__entity__view_callback($entities, $view_mode = 'full', $langcode = NULL) {
  $build = array();

  foreach ($entities as $entity) {
    $entity_type_name = $entity->entityType();
    $bundle_name = $entity->type;

    $entity_type = entity_type_load($entity_type_name);
    $bundle = bundle_load($entity_type_name, $bundle_name);

    $entity_view = eck__entity__build($entity_type, $bundle, $entity, $view_mode);
    $property_view = array();

    $formatters = eck_property_behavior_invoke_plugin($entity_type, 'default_formatter',
      array('entity' => $entity));

    foreach ($formatters as $property => $formatter) {
      $property_view[$property] = $formatter;
    }

    $entity_view = array_merge($property_view, $entity_view[$entity_type_name][$entity->id]);

    eck_property_behavior_invoke_plugin($entity_type, 'entity_view',
      array('entity' => $entity));

    $build[$entity_type_name][$entity->id] = $entity_view;
  }

  return $build;
}

/**
 * Get the value for a property from a form's state.
 */
function _eck_form_property_value($state, $property) {
  if (array_key_exists($property, $state['values'])) {
    return $state['values'][$property];
  }
  return NULL;
}
