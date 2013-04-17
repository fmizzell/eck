<?php

/**
 * @file
 * Contains the hooks for implementing ECK's default property widgets.
 * 
 * @see eck_property_info_widget_types().
 * @see hook_eck_property_widget_info().
 * @see hook_eck_property_widget_form().
 * @see hook_eck_property_widget_settings_form().
 */

/**
 * Implements hook_eck_property_widget_settings_form().
 * 
 * Returns the widget settings forms for various ECK defined property widgets.
 * 
 * @see eck_eck_property_widget_info().
 * @see hook_eck_property_widget_settings_form().
 */
function eck_eck_property_widget_settings_form($entity_type, $bundle, $property_name, $bundle_property_config) {
  $form = array();

  if ($bundle_property_config['widget']['type'] == 'text') {
    $settings = $bundle_property_config['widget']['settings'];
    // The size of the 'text' widget text box in columns/characters.
    $form['size'] = array(
      '#type' => 'textfield',
      '#title' => t('Size of textfield'),
      '#default_value' => $settings['size'],
      '#required' => TRUE,
      '#element_validate' => array('element_validate_integer_positive'),
    );
    // The maximum length for a 'text' widget
    $form['max_length'] = array(
      '#type' => 'textfield',
      '#title' => t('Maximum length'),
      '#default_value' => $settings['max_length'],
      '#required' => TRUE,
      '#description' => t('The maximum length of the field in characters.'),
      '#element_validate' => array('element_validate_integer_positive'),
    );
  }else if ($bundle_property_config['widget']['type'] == 'options') {
    $settings = $bundle_property_config['widget']['settings'];
    // The size of the 'text' widget text box in columns/characters.
    $form['options'] = array(
      '#type' => 'textarea',
      '#title' => t('Options'),
      '#default_value' => $settings['options'],
      '#required' => TRUE,
      '#description' => "Add the options, one per line, in the format value|label",
      '#element_validate' => array('eck_property_widget_options_validate'),
    );
  }

  return $form;
}

/**
 * Implements hook_eck_property_widget_form().
 * 
 * Returns the widget forms for various ECK defined property widgets. These
 * forms allow the user to update property data when creating/editing entities.
 * 
 * @see eck_eck_property_widget_info().
 * @see hook_eck_property_widget_form().
 * @see hook_eck_property_widget_settings_form().
 */
function eck_eck_property_widget_form(&$form, &$form_state, $property_name, $bundle_property_config, $language, $value, $element) {
  if ($bundle_property_config['widget']['type'] == 'text') {
    $element += array(
      '#type' => 'textfield',
      '#default_value' => isset($value) ? $value : NULL,
      '#size' => $bundle_property_config['widget']['settings']['size'],
      '#maxlength' => $bundle_property_config['widget']['settings']['max_length'],
      '#attributes' => array('class' => array('text-full')),
    );
  }
  if ($bundle_property_config['widget']['type'] == 'options') {
    $options = array();
    $options_string = $bundle_property_config['widget']['settings']['options'];
    if(!empty($options_string)){
      $option_lines = explode("\n", $options_string);

      foreach($option_lines as $line){
        $kv = explode("|", $line);
        $options[$kv[0]] = $kv[1];
      }
    }
    
    $element += array(
      '#type' => 'radios',
      '#default_value' => isset($value) ? $value : NULL,
      '#options' => $options
    );
  }
  return $element;
}

function eck_property_widget_options_validate($element, &$form_state) {
  $value = $element['#value'];
  $options = explode("\n", $value);
  foreach($options as $option){
    $key_value = explode("|", $option);
    if(count($key_value) != 2){
      form_error($element, t('%name: incorrect format.', array('%name' => $element['#title'])));
    }
  }
}