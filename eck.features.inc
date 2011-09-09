<?php

/**
 * @files
 * ECK Features integration.
 */

/**
 * Implements hook_features_export_options().
 */
function eck_features_export_options() {
  $types = array();
  $result = db_select("eck_types", 't')->fields('t')->execute();
  foreach ($result as $type) {
    $types[$type->type] = $type->label;
  }
  return $types;
}

/**
 * Implements hook_features_export().
 */
function eck_features_export($data, &$export, $module_name = '') {
  $pipe = array();

  foreach ($data as $type) {

    // Get the entity name of type.
    $instances = field_info_instances();
    foreach ($instances as $name => $bundles) {
      if (isset($bundles[$type])) {
        $entity_name = $name;
        break;
      }
    }

    // Export Entity.
    $export['features']['eck'][$type] = $type;
    $export['dependencies']['eck'] = 'eck';
    $export['dependencies']['features'] = 'features';

    // Export fields.
    $fields = $instances[$entity_name][$type];
    foreach ($fields as $field) {
      $pipe['field'][] = "{$entity_name}-{$field['bundle']}-{$field['field_name']}";
    }
  }

  return $pipe;
}

/**
 * Implements hook_features_export_render().
 */
function eck_features_export_render($module, $data, $export = NULL) {
  $elements = array(
    'entity' => FALSE,
    'type' => FALSE,
    'label' => TRUE,
  );
  $output = array();
  $output[] = '  $items = array(';
  foreach ($data as $type) {
    $info = db_select("eck_types", 't')
            ->fields('t')
            ->condition('type', $type)
            ->execute()
            ->fetchObject();
    $output[] = "    '{$type}' => array(";
    foreach ($elements as $key => $t) {
      if ($t) {
        $text = str_replace("'", "\'", $info->$key);
        $text = !empty($text) ? "t('{$text}')" : "''";
        $output[] = "      '{$key}' => {$text},";
      }
      else {
        $output[] = "      '{$key}' => '{$info->$key}',";
      }
    }
    $output[] = "    ),";
  }
  $output[] = '  );';
  $output[] = '  return $items;';
  $output = implode("\n", $output);
  return array('eck_info' => $output);
}

/**
 * Implements hook_features_revert().
 */
function eck_features_revert($module) {
  eck_features_rebuild($module);
}

/**
 * Implements of hook_features_rebuild().
 *
 * Rebuilds eck entities from code defaults.
 */
function eck_features_rebuild($module) {
  if ($default_types = features_get_default('eck', $module)) {
    foreach ($default_types as $type_name => $type_info) {
      db_delete('eck_types')
          ->condition('type', $type_name)
          ->execute();

      db_insert('eck_types')
          ->fields(array(
            'entity' => $type_info['entity'],
            'type' => $type_name,
            'label' => $type_info['label'],
          ))
          ->execute();

      db_delete('eck')
          ->condition('name', $type_info['entity'])
          ->execute();

      db_insert('eck')
          ->fields(array(
            'name' => $type_info['entity'],
            'label' => $type_info['label'],
          ))
          ->execute();

      if (!db_table_exists("eck_{$type_info['entity']}")) {
        db_create_table("eck_{$type_info['entity']}", eck__entity__schema($type_info['entity']));
      }
    }

    drupal_get_schema(NULL, TRUE);
    entity_info_cache_clear();
    menu_rebuild();
  }
}
