<?php


/**
 * Respond to the creation of a new ECK entity type.
 *
 * @param EntityType $entity_type
 *   The entity type is being created.
 */
function hook_eck_entity_type_insert(EntityType $entity_type) {

}

/**
 * Respond to the updating of a new ECK entity type.
 *
 * @param EntityType $entity_type
 *   The entity type is being update.
 */
function hook_eck_entity_type_update(EntityType $entity_type) {

}

/**
 * Respond to the deletion of a new ECK entity type.
 *
 * @param EntityType $entity_type
 *   The entity type is being deleted.
 */
function hook_eck_entity_type_delete(EntityType $entity_type) {

}

/**
 * Defines ECK property widget types.
 * 
 * @return
 *   An array of widget type definitions. The individual keys are the widget
 *   types. Each definition is an array with the following optional keys:"
 * 
 *     'label': The human readable name of the widget type.
 *     'property types': The ECK property types that the widget applies to. i.e.
 *                       text, integer, decimal, positive_integer, language.
 *     'settings': An array of the default settings for the widget if it has any
 *                 settings.
 *     'file': A file that should be included when processing widget forms. This
 *             file may contain all of the ECK property widget api hooks. For
 *             example defining the widget settings forms and the widget forms.
 *     'file type': The type of file for the 'file' above. Default is 'inc'.
 *     'description': Description to use as help text for property widget
 *                    selection.
 *     'value callback': The name of a callback function to use for processing
 *                       the value returned by the widget before saving. See
 *                       eck_property_widget_extract_value().
 *     
 **/
function hook_eck_property_widget_info() {
  // Define a simple text widget type.
  $widget_types = array(
    'text' => array(
      'label' => t('Text'),
      'settings' => array('size' => 60, 'max_length' => 255),
      'property types' => array('text'),
      'file' => 'eck.property_widgets',
    ),
  );
  return $widget_types;
}

/**
 * Alters the widget type info returned by hook_eck_property_widget_info().
 */
function hook_eck_property_widget_info(&$widget_types) {
  // Change the label on the text widget type.
  $widget_types['text']['label'] = t('Property text box');
  // Add a newly defined property type to the allowed property types for the text widget. 
  $widget_types['text']['property types'] += array('mycoolnewpropertytype');
}

/**
 * Callback to retrieve the form elements for a module's defined property
 * widgets. This same hook will be called for every property widget type
 * defined by a given module.
 * 
 * This callback operates very similar to that of Drupal's field api hooks.
 * 
 * The form may be altered using hook_eck_property_widget_form() and hook_eck_property_widget_WIDGET_TYPE_form().
 * 
 * @param $form
 *   A reference to the parent form. Could be the entity form, the widget
 *   settings form, etc.
 * @param form_state
 *   A reference to the current state of the form.
 * @param property_name
 *   The machine name of the property for which to retreive the widget form.
 * @param bundle_property_config
 *   The bundle's configuration setting stored for the property. Contains all of
 *   the widget's settings and default info for the property.
 * @param langcode
 *   The current language.
 * @param $value
 *   The property's current value to use in the widget.
 * @param $element
 *   The form element to use for the property widget. Default info included.
 *  
 * @return the form element for a particular widget.
 * 
 * @see eck_eck_property_widget_info().
 * @see hook_eck_property_widget_form().
 * @see hook_eck_property_widget_settings_form().
 * @see hook_eck_property_widget_form().
 * @see hook_eck_property_widget_WIDGET_TYPE_form().
 */
function hook_eck_property_widget_form(&$form, &$form_state, $property_name, $bundle_property_config, $langcode, $value, $element)) {
  if ($bundle_property_config['widget']['type'] == 'text') {
    $element += array(
      '#type' => 'textfield',
      '#default_value' => isset($value) ? $value : NULL,
      '#size' => $bundle_property_config['widget']['settings']['size'],
      '#maxlength' => $bundle_property_config['widget']['settings']['max_length'],
      '#attributes' => array('class' => array('text-full')),
    );
  }
  return $element;
}

/**
 * Alters the property widget form. Called for every widget type.
 * 
 * @param $element
 *   A reference to the property widget's form element.
 * @param form_state
 *   A reference to the current state of the form.
 * @param $context
 *   An array containing contextual information.
 */
function hook_eck_property_widget_form(&$element, $form_state, $context) {
}

/**
 * Alters the property widget form. Called for the specific WIDGET_TYPE widget.
 * 
 * @param $element
 *   A reference to the property widget's form element.
 * @param form_state
 *   A reference to the current state of the form.
 * @param $context
 *   An array containing contextual information:
 *     'form': the parent form. Could be the entity form, the widget
 *             settings form, etc.
 *     'property_name': The machine name of the property for which to retreive
 *                      the widget form.
 *     'bundle_property_config': The bundle's configuration setting stored for
 *                               the property. Contains all of the widget's
 *                               settings and default info for the property.
 *     'langcode': The current language.
 *     'value': The property's current value to use in the widget.
 */
function hook_eck_property_widget_WIDGET_TYPE_form(&$element, $form_state, $context) {
}
