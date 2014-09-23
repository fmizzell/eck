<?php
/**
 * @file
 * Entity Reference behavior definition.
 */

$plugin = array(
  'label' => "Entity Reference",
  'config_form' => 'eck_entity_reference_property_config_form',
  'property_info' => 'eck_entity_reference_property_property_info',
);

/**
 * Configuration form for behaviors.
 */
function eck_entity_reference_property_config_form() {
  $entity_types = EntityType::loadAll();
  $options = array();
  foreach ($entity_types as $et) {
    $options[$et->name] = $et->label;
  }

  $form['entity_type'] = array(
    '#type' => 'radios',
    '#options' => $options,
    '#required' => TRUE,
  );

  return $form;
}

/**
 * Property metadata for entity references.
 */
function eck_entity_reference_property_property_info($property, $vars) {

  $config = $vars['config'];
  $vars['properties'][$property]['type'] = $config['entity_type'];
  return $vars;
}
