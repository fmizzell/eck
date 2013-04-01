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
 *                       the value returned by the widget before saving.
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
