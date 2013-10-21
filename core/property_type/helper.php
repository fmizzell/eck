<?php

function eck_get_property_types($property_type = '', $reset = FALSE){
  global $language;
  static $property_types;

  // The _info() hooks invoked below include translated strings, so each
  // language is cached separately.
  $langcode = $language->language;

  if ($reset) {
    $property_types = NULL;
    // Clear all languages.
    cache_clear_all('property_types:', 'cache_eck', TRUE);
  }

  if (!$property_types) {
    if ($cached = cache_get("property_types:$langcode", 'cache_eck')) {
      $property_types = $cached->data;
    }
    else {
      $property_types = array();

      // Populate property widget types.
      foreach (module_implements('eck_property_types') as $module) {
        $module_property_types = (array) module_invoke($module, 'eck_property_types');
        foreach ($module_property_types as $name => $property_type_info) {
          // Provide defaults.
          $property_type_info += array(
            'type' => $name,
            'label' => t($name),
            'settings' => array(),
            'class' => FALSE,
            'file' => FALSE,
            'file type' => 'inc',
            'description' => '',
          );
          $property_types[$name] = $property_type_info;
          $property_types[$name]['module'] = $module;
        }
      }
      drupal_alter('eck_property_types', $property_types);

      cache_set("property_types:$langcode", $property_types, 'cache_eck');
    }
  }

  if (!$property_type) return $property_types;
  
  if (isset($property_types[$property_type])) return $property_types[$property_type];
}

/**
 * Get the class for a specific property type.
 *
 * @param $property_type
 *   The property type to retreive the class for.
 *
 * @return
 *   An instance of the property type class or FALSE if the class could not be loaded.
 *
 * @see eck_get_property_types().
 */
function eck_get_property_type_class($property_type_name) {
  $property_types = module_invoke_all('eck_property_types');
  
  if(array_key_exists($property_type_name, $property_types)){
    return $property_types[$property_type_name]['class'];
  }else{
    return FALSE;
  }
}

function eck_get_property_type($property_type){
  return eck_get_property_types($property_type);
}

function eck_get_property_type_schema($property_type_name){
  if ($class = eck_get_property_type_class($property_type_name)) {
    return $class::schema();
  }
}
