<?php
/**
 * @file
 * Properties.
 */

/**
 * Properties page.
 *
 * @param string $entity_type_name
 *   The name of an entity type.
 *
 * @return array
 *   The page build.
 */
function eck__properties($entity_type_name) {
  $current_path = current_path();

  $entity_type = EntityType::loadByName($entity_type_name);

  // Lets get the default properties.
  $default_properties = eck_get_default_properties();

  // Let get the actual properties of the entity type.
  $entity_type_properties = $entity_type->properties;

  // Lets arrenge the table with the appropiate action links.

  // This is the table where all properties are shown.
  $header = array(
    'machine_name' => t('Machine Name'),
    'name' => t('Name'),
    'type' => t('Type'),
    'behavior' => t('Behaviour'),
    'operations' => t('Operations'),
  );

  $options = array();

  // Lets process default properties first.
  foreach ($default_properties as $machine_name => $info) {
    $default_property_row = array();
    $info['machine_name'] = $machine_name;
    foreach ($header as $key => $label) {
      if ($key == 'name') {
        $key = "label";
      }
      if ($key != 'operations') {
        $default_property_row[] = array_key_exists($key, $info) ? $info[$key] : "";
      }
    }

    if (array_key_exists($machine_name, $entity_type_properties)) {
      $default_property_row[] = l(t('Deactivate'), $current_path . "/" . $machine_name . "/deactivate");
      unset($entity_type_properties[$machine_name]);
    }
    else {
      $link = "{$current_path}/{$machine_name}/activate";
      $default_property_row[] = l(t('Activate'), $link);
    }

    $options[] = $default_property_row;
  }

  // Ok, now lets do our custom properties.
  foreach ($entity_type_properties as $machine_name => $info) {
    $default_property_row = array();
    $info['machine_name'] = $machine_name;

    foreach ($header as $key => $label) {
      if ($key == 'name') {
        $key = "label";
      }
      if ($key != 'operations') {
        $default_property_row[] = array_key_exists($key, $info) ? $info[$key] : "";
      }
    }

    $operations = "";
    $ops = array(
      'edit' => "Edit",
      'delete' => "Delete",
      "behavior" => "behavior",
    );

    foreach ($ops as $op => $label) {
      if ($op == 'behavior' && !empty($info['behavior'])) {
        $label = "Edit " . $label;
      }
      elseif ($op == 'behavior' && empty($info['behavior'])) {
        $label = "Add " . $label;
      }

      $operations .= l($label, $current_path . "/" . $machine_name . "/{$op}") . "<br \>";
    }

    $default_property_row[] = $operations;

    $options[] = $default_property_row;
  }

  $build['properties_table'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $options,
    '#empty' => t('No other properties for this entity type.'),
  );

  return $build;
}

/**
 * Add property form.
 */
function eck__property_add__form($form, &$state, $entity_type_name) {

  $form['entity_type_name'] = array(
    '#type' => 'value',
    '#value' => $entity_type_name,
  );

  // Add new property.
  $property_types = eck_get_property_types();

  // Lets transform the property_types info array into something appropriate
  // for a select list.
  $property_type_options = array();
  foreach ($property_types as $property_name => $info) {
    $property_type_options[$property_name] = $info['label'];
  }

  $types[t('Generic')] = $property_type_options;

  $form["add_new_property"] = array(
    '#markup' => '<h3>Add new property</h3>',
  );

  $form['property_type'] = array(
    '#type' => 'select',
    '#title' => t('Type'),
    '#options' => array('' => t('- Please choose -')) + $types,
    '#required' => TRUE,
  );

  $form['property_select_type'] = array(
    '#type' => 'submit',
    '#value' => t('Select Property Type'),
  );

  if (array_key_exists('values', $state) && !empty($state['values']['property_type'])) {

    $property_type = $state['values']['property_type'];
    $property_type_info = eck_get_property_types($property_type);

    $default_schema = $property_type_info['class']::schema();

    $form['default_schema'] = array(
      '#type' => 'value',
      '#value' => $default_schema,
    );

    $sf = new SchemaForm($default_schema);

    $schema_form = $sf->getSchemaForm();
    foreach ($schema_form as $name => $field) {
      unset($schema_form[$name]);
      $schema_form["schema_{$name}"] = $field;
    }

    $form['schema'] = array(
      '#type' => 'fieldset',
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#title' => 'Schema Configuration',
    );

    $form['schema']['form'] = $schema_form;

    $form["property_label"] = array(
      '#type' => 'textfield',
      '#title' => t("Name"),
      '#description' => t("A human readable name for the property."),
      '#required' => TRUE,
      '#after_build' => array('_eck_deactivate_on_save'),
    );

    $form["property_name"] = array(
      '#type' => 'machine_name',
      '#machine_name' => array(
        'exists' => '_eck_fake_exists',
        'source' => array('property_label'),
      ),
      '#after_build' => array('_eck_deactivate_on_save'),
    );

    $form['property_add'] = array(
      '#type' => 'submit',
      '#value' => t('Add Property'),
    );

  }

  return $form;
}

/**
 * Property edit form.
 */
function eck__property_edit__form($form, &$state, $entity_type_name, $property) {
  $entity_type = EntityType::loadByName($entity_type_name);
  $properties = $entity_type->properties;
  if (array_key_exists($property, $properties)) {
    $form['entity_type'] = array(
      '#type' => 'value',
      '#value' => $entity_type,
    );

    $form['property'] = array(
      '#type' => 'value',
      '#value' => $property,
    );

    $form['label'] = array(
      '#type' => 'textfield',
      '#title' => 'label',
      '#default_value' => $properties[$property]['label'],
    );

    $form['submit'] = array(
      '#type' => 'submit',
      '#weight' => 10000,
      '#value' => t('Save'),
    );
  }
  else {
    drupal_set_message('This property does not exists');
    drupal_goto(eck__entity_type__path() . "/{$entity_type->name}/property");
  }

  return $form;
}

/**
 * Property edit form submit.
 */
function eck__property_edit__form_submit($form, &$state) {
  $values = $state['values'];
  $entity_type = $values['entity_type'];
  $property = $values['property'];
  $label = $values['label'];

  $properties = $entity_type->properties;
  $properties[$property]['label'] = $label;

  $entity_type->properties = $properties;

  $entity_type->save();

  EntityType::loadAll(NULL, TRUE);
  $state['redirect'] = eck__entity_type__path() . "/{$entity_type->name}/property";
}

/**
 * Property delete form.
 */
function eck__property_delete__form($form, &$state, $entity_type_name, $property) {
  $entity_type = EntityType::loadByName($entity_type_name);
  $properties = $entity_type->properties;
  if (array_key_exists($property, $properties)) {
    $form['entity_type'] = array(
      '#type' => 'value',
      '#value' => $entity_type,
    );

    $form['property'] = array(
      '#type' => 'value',
      '#value' => $property,
    );

    $form['confirmation'] = confirm_form($form,
     "Are you sure you want to delete {$property}",
     eck__entity_type__path() . "/{$entity_type->name}/property");

  }
  else {
    // @todo are we really using drupal_goto?
    drupal_set_message('This property does not exists');
    drupal_goto(eck__entity_type__path() . "/{$entity_type->name}/property");
  }

  return $form;
}

/**
 * Property delete form submit.
 */
function eck__property_delete__form_submit($form, &$state) {
  $values = $state['values'];
  $entity_type = $values['entity_type'];
  $property = $values['property'];

  $entity_type->removeProperty($property);

  $entity_type->save();

  EntityType::loadAll(NULL, TRUE);
  $state['redirect'] = eck__entity_type__path() . "/{$entity_type->name}/property";
}

/**
 * Property add form.
 */
function eck__property_add__form_submit($form, &$state) {
  $values = $state['values'];

  if (strcmp($values['op'], t('Select Property Type')) == 0) {
    $state['rebuild'] = TRUE;
  }
  else {
    // Lets save schema customization.
    $default_schema = $values['default_schema'];
    unset($values['default_schema']);

    $new_schema = array();

    // Lets get the values from the form.
    foreach ($values as $name => $value) {
      if (substr_count($name, "schema") > 0) {
        $new_name = str_replace("schema_", "", $name);
        if (strcmp($new_name, "unsigned") == 0 || strcmp($new_name, "not null") == 0) {
          $value = ($value) ? TRUE : FALSE;
        }
        $new_schema[$new_name] = $value;
      }
    }

    // If the default is empty lets take it off.
    if (array_key_exists('default', $new_schema) && empty($new_schema['default'])) {
      unset($new_schema['default']);
    }

    // Lets format the not null key to work with schema.
    if (array_key_exists('not_null', $new_schema)) {
      $temp = $new_schema['not_null'];
      unset($new_schema['not_null']);
      $new_schema['not null'] = $temp;
    }

    $entity_type_name = $values['entity_type_name'];
    $entity_type = EntityType::loadByName($entity_type_name);

    $entity_type->addProperty($values['property_name'], $values['property_label'],
            $values['property_type'], NULL, $new_schema);

    $entity_type->save();
    EntityType::loadAll(NULL, TRUE);
  }
}

/**
 * Property config.
 */
function eck_properties_property_config($form, &$state) {
  return $form['eck_property_config2'];
}

/**
 * Properties form validation callback.
 */
function eck__properties__form_validate($form, &$state) {
  if ($state['values']['op'] == t('Add Property')) {
    if (isset($state['values']['property_behavior'])) {
      $property_behavior = $state['values']['property_behavior'];
      $behavior_plugins = ctools_get_plugins('eck', 'property_behavior');
      if (isset($behavior_plugins[$property_behavior]['unique']) && $behavior_plugins[$property_behavior]['unique']) {
        foreach ($state['values']['new_properties'] as $property_name => $property_info) {
          if ($property_info['behavior'] == $property_behavior) {
            form_set_error('property_behavior',
              t("There can be only one property with '@name' behavior per entity.",
                array('@name' => $behavior_plugins[$property_behavior]['label'])));
          }
        }
      }
    }
  }
}

/**
 * Properties form submit callback.
 */
function eck__properties__form_submit($form, &$state) {
  if ($state['values']['op'] == t('Add Property')) {
    $state['rebuild'] = TRUE;
  }
  elseif ($state['values']['op'] == t('Save')) {
    // Here we want to add the properties to the entity type and save it.
    $entity_type = $state['values']['entity_type'];

    foreach ($state['values']['new_properties_table'] as $property => $active) {
      if ($active) {
        $info = $state['values']['new_properties'][$property];
        if (array_key_exists('behavior', $info)) {
          $entity_type->addProperty($property, $info['label'], $info['type'], $info['behavior']);
        }
        else {
          $entity_type->addProperty($property, $info['label'], $info['type']);
        }
      }
      else {
        $entity_type->removeProperty($property);
      }
    }

    $entity_type->save();

    // Lets flush the cache so new behaviors and properties will get set up
    // correctly.
    // @todo do we need to replace this with our new caching strategy?
    drupal_flush_all_caches();
  }
}

/**
 * Deactivate on save.
 */
function _eck_deactivate_on_save($element, &$state) {
  if (array_key_exists('input', $state) && array_key_exists('op', $state['input']) &&
    $state['input']['op'] == t('Save')) {
    isset($element['#element_validate']) ? $element['#element_validate'] = NULL : NULL;
    isset($element['#needs_validation']) ? $element['#needs_validation'] = NULL : NULL;
  }

  return $element;
}
