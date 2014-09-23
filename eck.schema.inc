<?php
/**
 * @file
 * A helper class to create schema forms
 */

class SchemaForm {
  protected $lockedProperties;
  protected $defaultSchema;

  /**
   * SchemaForm constructor.
   *
   * @param array $default_schema
   *   A schema array as it is typical in drupal.
   * @param array $locked_properties
   *   A list of properties in the schema that are not up to change.
   */
  public function __construct($default_schema, $locked_properties = array()) {
    // The type property is always locked.
    $locked_properties['type'] = TRUE;
    $this->lockedProperties = $locked_properties;

    $this->defaultSchema = $default_schema;
  }

  /**
   * Helper function to turn a string to camel case.
   *
   * @param string $string
   *   A string.
   *
   * @return string
   *   The camel case version of the string.
   */
  protected function stringToCamelCase($string) {
    $pieces = explode(" ", $string);
    foreach ($pieces as $key => $piece) {
      if ($key > 0) {
        $pieces[$key] = ucfirst($piece);
      }
    }

    return implode("", $pieces);
  }

  /**
   * Generate a form api form from the given schema.
   *
   * @return array
   *   form api array.
   */
  public function getSchemaForm() {
    $form = array();
    foreach ($this->defaultSchema as $property => $value) {
      $method_prefix = $this->stringToCamelCase($property);
      $method = "{$method_prefix}FormElement";
      if ($property == "not null") {
        $property = "not_null";
      }
      $form[$property] = $this->{$method}($value);
    }
    return $form;
  }

  /**
   * Schmea form section for schema descriptions.
   *
   * @param string $value
   *   The current value
   *
   * @return array
   *   form api array piece.
   */
  protected function descriptionFormElement($value) {
    $key = "description";
    $element = array(
      '#title' => t('Description'),
      '#type' => 'textarea',
      '#default_value' => $value,
      '#disabled' => array_key_exists($key, $this->lockedProperties) ? $this->lockedProperties[$key] : FALSE,
    );
    return $element;
  }

  /**
   * Schmea form section for schema type.
   *
   * @param string $value
   *   The current value
   *
   * @return array
   *   form api array piece.
   */
  protected function typeFormElement($value) {
    $key = "type";
    $element = array(
      '#title' => t('Type'),
      '#type' => 'textfield',
      '#default_value' => $value,
      '#disabled' => array_key_exists($key, $this->lockedProperties) ? $this->lockedProperties[$key] : FALSE,
    );
    return $element;
  }

  /**
   * Schmea form section for schema size.
   *
   * @param string $value
   *   The current value
   *
   * @return array
   *   form api array piece.
   */
  protected function sizeFormElement($value) {
    $key = "size";

    $options = array(
      'tiny' => 'tiny',
      'small' => 'small',
      'medium' => 'medium',
      'normal' => 'normal',
      'big' => 'big',
    );

    $element = array(
      '#title' => t('Size'),
      '#type' => 'select',
      '#options' => $options,
      '#default_value' => $value,
      '#disabled' => array_key_exists($key, $this->lockedProperties) ? $this->lockedProperties[$key] : FALSE,
    );
    return $element;
  }

  /**
   * Schmea form section for schema notNull.
   *
   * @param string $value
   *   The current value
   *
   * @return array
   *   form api array piece.
   */
  protected function notNullFormElement($value) {
    $key = "not null";

    $element = array(
      '#title' => t('Not Null'),
      '#type' => 'checkbox',
      '#default_value' => $value,
      '#disabled' => array_key_exists($key, $this->lockedProperties) ? $this->lockedProperties[$key] : FALSE,
    );
    return $element;
  }

  /**
   * Schmea form section for schema default.
   *
   * @param string $value
   *   The current value
   *
   * @return array
   *   form api array piece.
   */
  protected function defaultFormElement($value) {
    $key = "default";

    $element = array(
      '#title' => t('Default'),
      '#type' => 'textfield',
      '#default_value' => $value,
      '#disabled' => array_key_exists($key, $this->lockedProperties) ? $this->lockedProperties[$key] : FALSE,
    );
    return $element;
  }

  /**
   * Schmea form section for schema unsigned.
   *
   * @param string $value
   *   The current value
   *
   * @return array
   *   form api array piece.
   */
  protected function unsignedFormElement($value) {
    $key = "unsigned";

    $element = array(
      '#title' => t('Unsigned'),
      '#type' => 'checkbox',
      '#default_value' => $value,
      '#disabled' => array_key_exists($key, $this->lockedProperties) ? $this->lockedProperties[$key] : FALSE,
    );
    return $element;
  }

  /**
   * Schmea form section for schema length.
   *
   * @param string $value
   *   The current value
   *
   * @return array
   *   form api array piece.
   */
  protected function lengthFormElement($value) {
    $key = "default";

    $element = array(
      '#title' => t('Length'),
      '#type' => 'textfield',
      '#default_value' => $value,
      '#disabled' => array_key_exists($key, $this->lockedProperties) ? $this->lockedProperties[$key] : FALSE,
    );
    return $element;
  }
}
