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
 * @see eck_eck_property_widget_info()
 * @see hook_eck_property_widget_settings_form()
 */
function eck_eck_property_widget_settings_form($entity_type, $bundle, $property_name, $bundle_property_config) {
  $widget = (isset($bundle_property_config['widget'])) ? $bundle_property_config['widget'] : array('type' => '');
  if ($widget['type'] == 'text') {
    return array(
      // The size of the 'text' widget text box in columns/characters.
      'size' => array(
        '#type' => 'textfield',
        '#title' => t('Size of textfield'),
        '#default_value' => $widget['settings']['size'],
        '#required' => TRUE,
        '#element_validate' => array('element_validate_integer_positive'),
      ),
      // The maximum length for a 'text' widget.
      'max_length' => array(
        '#type' => 'textfield',
        '#title' => t('Maximum length'),
        '#default_value' => $widget['settings']['max_length'],
        '#required' => TRUE,
        '#description' => t('The maximum length of the field in characters.'),
        '#element_validate' => array('element_validate_integer_positive'),
      ),
    );
  }
  elseif ($widget['type'] == 'options') {
    return array(
      // The allowed options for the options widget.
      'options' => array(
        '#type' => 'textarea',
        '#title' => t('Options'),
        '#default_value' => $widget['settings']['options'],
        '#required' => TRUE,
        '#description' => t('Add the options, one per line, in the format value|label'),
        '#element_validate' => array('eck_property_widget_options_validate'),
      ),
    );
  }
}

/**
 * Implements hook_eck_property_widget_form().
 *
 * Returns the widget forms for various ECK defined property widgets. These
 * forms allow the user to update property data when creating/editing entities.
 *
 * @see eck_eck_property_widget_info()
 * @see hook_eck_property_widget_form()
 * @see hook_eck_property_widget_settings_form()
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
    if (!empty($options_string)) {
      $option_lines = explode("\n", $options_string);

      foreach ($option_lines as $line) {
        $kv = explode("|", $line);
        $options[$kv[0]] = $kv[1];
      }
    }

    $element += array(
      '#type' => 'radios',
      '#default_value' => isset($value) ? $value : NULL,
      '#options' => $options,
    );
  }
  if ($bundle_property_config['widget']['type'] == 'language_toggle') {
    global $language;
    if (module_exists('locale')) {
      $element += array(
        '#type' => 'select',
        '#default_value' => isset($value) ? $value : $language->language,
        '#options' => array(LANGUAGE_NONE => t('Language neutral')) + locale_language_list('name'),
      );
    }
    else {
      $element += array(
        '#type' => 'value',
        '#value' => !isset($value) ? $value : $language->language,
      );
    }
  }
  return $element;
}

/**
 * Widget validate.
 */
function eck_property_widget_options_validate($element, &$form_state) {
  $value = $element['#value'];
  $options = explode("\n", $value);
  foreach ($options as $option) {
    $key_value = explode("|", $option);
    if (count($key_value) != 2) {
      form_error($element, t('%name: incorrect format.', array('%name' => $element['#title'])));
    }
  }
}
