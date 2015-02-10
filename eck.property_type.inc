<?php
/**
 * @file
 * Property types.
 */

/**
 * Get property types.
 */
function eck_get_property_types($property_type = '', $reset = FALSE) {
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
            'label' => t("@name", array("@name" => $name)),
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

  if (!$property_type) {
    return $property_types;
  }

  if (isset($property_types[$property_type])) {
    return $property_types[$property_type];
  }
}

/**
 * Get the class for a specific property type.
 *
 * @param string $property_type_name
 *   The property type to retreive the class for.
 *
 * @return object
 *   An instance of the property type class or FALSE if the class could not be
 *   loaded.
 *
 * @see eck_get_property_types()
 */
function eck_get_property_type_class($property_type_name = '') {
  $property_classes = &drupal_static(__FUNCTION__, array());
  $load_types = NULL;
  if ($property_type_name && empty($property_classes[$property_type_name])) {
    // Just load the class of the property type we're looking for.
    if ($property_type = eck_get_property_types($property_type_name)) {
      $load_types = array($property_type_name => $property_type);
    }
  }
  elseif (!$property_type_name) {
    // Load all the classes we haven't yet loaded.
    $load_types = array_diff_key(eck_get_property_types(), $property_classes);
  }
  if ($load_types) {
    // Loop through all the property type classes to load and load them.
    // if they exist.
    foreach ($load_types as $property_type => $property_type_info) {
      if ($class = $property_type_info['class']) {
        $class_loaded = class_exists($class);
        if (!$class_loaded) {
          if (function_exists('drupal_get_path') && $property_type_info['file']) {
            $file = DRUPAL_ROOT . '/' . drupal_get_path('module', $property_type_info['module']) . '/' . $property_type_info['file'];
            if (is_file($file)) {
              require_once $file;
              $class_loaded = class_exists($class);
            }
          }
        }
        $property_classes[$property_type] = ($class_loaded) ? $class : FALSE;
      }
    }
  }
  // Return an array of all the property type classes.
  if (!$property_type_name) {
    return $property_classes;
  }

  // Only return the class for a specific property type.
  if (isset($property_classes[$property_type_name])) {
    return $property_classes[$property_type_name];
  }
}

/**
 * Get property type.
 */
function eck_get_property_type($property_type) {
  return eck_get_property_types($property_type);
}

/**
 * Get property type schema.
 */
function eck_get_property_type_schema($property_type_name) {
  if ($class = eck_get_property_type_class($property_type_name)) {
    return $class::schema();
  }
}

/**
 * Generic types supported by schema api
 * 'varchar', 'char', 'int', 'serial', 'float', 'numeric', 'text', 'blob' or
 * 'datetime'
 */
interface PropertyTypeInterface {
  /**
   * Schema.
   */
  public static function schema();

  /**
   * Validate.
   */
  public static function validate($value);
}

/**
 * PRIMITIVE PROPERTY TYPES
 */

class TextPropertyType implements PropertyTypeInterface {

  /**
   * Schema.
   */
  public static function schema() {
    $schema = array(
      'description' => 'Text',
      'type' => 'varchar',
      'not null' => TRUE,
      'default' => '',
      // Relevant only to: varchar, char, and text.
      'length' => 255,
    );

    return $schema;
  }

  /**
   * Validate.
   */
  public static function validate($value) {
    // Pretty much anything can be store in a text field even an object?
    // @TODO check what happens when trying to save an obejct to a varchar field
    // in the db.

    return TRUE;
  }
}

class FixedSizeTextPropertyType implements PropertyTypeInterface {

  /**
   * Schema.
   */
  public static function schema() {
    $schema = array(
      'description' => 'All of the  values of this property are text of the same size',
      'type' => 'char',
      'not null' => TRUE,
      'default' => '',
      // Relevant only to: varchar, char, and text.
      'length' => 255,
    );

    return $schema;
  }

  /**
   * Validate.
   */
  public static function validate($value) {
    // Pretty much anything can be store in a text field
    // even an object?
    // @TODO check what happens when trying to save an obejct to a varchar field
    // in the db.

    return TRUE;
  }
}

class LongTextPropertyType implements PropertyTypeInterface {

  /**
   * Schema.
   */
  public static function schema() {
    $schema = array(
      'description' => 'Long Text (longer than 8000 characters)',
      'type' => 'text',
      'length' => 255,
      'not null' => TRUE,
      // Relevant only to: varchar, char, and text.
      'length' => 255,
    );

    return $schema;
  }

  /**
   * Validate.
   */
  public static function validate($value) {
    // Pretty much anything can be store in a text field
    // even an object?
    // @TODO check what happens when trying to save an obejct to a varchar field
    // in the db.

    return TRUE;
  }
}

class IntegerPropertyType implements PropertyTypeInterface {

  /**
   * Schema.
   */
  public static function schema() {
    $schema = array(
      'description' => "Integer",
      'type' => 'int',
      'size' => 'normal',
      'not null' => TRUE,
      'default' => 0,
      // Relevant only to int and float.
      'unsigned' => FALSE,
    );

    return $schema;
  }

  /**
   * Validate.
   */
  public static function validate($value) {
    $number = filter_var($value, FILTER_VALIDATE_INT);
    return ($number !== FALSE);
  }
}

class DecimalPropertyType implements PropertyTypeInterface {

  /**
   * Schema.
   */
  public static function schema() {
    $schema = array(
      'description' => 'Decimal/Float/Double',
      'type' => 'float',
      'not null' => TRUE,
      'default' => 0,
      // Relevant only to int and float.
      'unsigned' => FALSE,
    );

    return $schema;
  }

  /**
   * Validate.
   */
  public static function validate($value) {
    $number = filter_var($value, FILTER_VALIDATE_FLOAT);
    return ($number !== FALSE);
  }
}

class BlobPropertyType implements PropertyTypeInterface {

  /**
   * Schema.
   */
  public static function schema() {
    $schema = array(
      'description' => 'Decimal/Float/Double',
      'type' => 'blob',
      'size' => 'normal',
      'not null' => TRUE,
    );

    return $schema;
  }

  /**
   * Validate.
   */
  public static function validate($value) {
   // Anything can be stored in a blob.
    return TRUE;
  }
}

class DateTimePropertyType implements PropertyTypeInterface {

  /**
   * Schema.
   */
  public static function schema() {
    $schema = array(
      'description' => 'Date/Time',
      'type' => 'datetime',
      'not null' => TRUE,
      'default' => 0,
    );

    return $schema;
  }

  /**
   * Validate.
   */
  public static function validate($value) {
    $number = filter_var($value, FILTER_VALIDATE_FLOAT);
    return ($number !== FALSE);
  }
}

/**
 * SPECIAL PROPERTY TYPES
 */

class LanguagePropertyType extends TextPropertyType {

  /**
   * Schema.
   */
  public static function schema() {
    $schema = parent::schema();

    $schema['default'] = LANGUAGE_NONE;
    $schema['description'] = "Language";
    $schema['length'] = 12;

    return $schema;
  }

  /**
   * Validate.
   */
  public static function validate($value) {
    // Make sure it is text.
    if (parent::validate($value)) {
      // @TODO check with the language people, what kind of validation can we
      // do here.
      return TRUE;
    }

    return FALSE;
  }
}

class UUIDPropertyType extends FixedSizeTextPropertyType {

  /**
   * Schema Conditions.
   */
  public static function schemaConditions() {
    return array('locked' => 'length');
  }

  /**
   * Schema.
   */
  public static function schema() {
    $schema = parent::schema();
    $schema['description'] = 'Universally Unique Identifier';
    $schema['length'] = 16;

    return $schema;
  }

  /**
   * Validate.
   */
  public static function validate($value) {
    // @TODO UUID module has a function ... steal :)
    return TRUE;
  }
}

class PositiveIntegerPropertyType extends IntegerPropertyType {

  /**
   * Schema.
   */
  public static function schema() {
    $schema = parent::schema();

    $schema['description'] = "Positive Integer";
    $schema['unsigned'] = TRUE;

    return $schema;
  }

  /**
   * Validate.
   */
  public static function validate($value) {
    if (parent::validate($value)) {

      // Lets cast it in case it is string.
      $int = intval($value);
      if ($int >= 0) {
        return TRUE;
      }
    }

    return FALSE;
  }
}
