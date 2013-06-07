<?php

/**
 * A helper class to create schema forms
 */

class SchemaForm{
  protected $locked_properties;
  protected $default_schema;
  
  public function __construct($default_schema, $locked_properties = array()){
    //the type property is always locked;
    $locked_properties['type'] = TRUE;
    $this->locked_properties = $locked_properties;
    
    $this->default_schema = $default_schema;
  }
  
  protected function stringToCamelCase($string){
    $pieces = explode(" ", $string);
    foreach($pieces as $key => $piece){
      if($key > 0){
        $pieces[$key] = ucfirst($piece);
      }
    }
    
    return implode("", $pieces);
  }
  
  public function getSchemaForm(){
    $form = array();
    foreach($this->default_schema as $property => $value){
      $method_prefix = $this->stringToCamelCase($property);
      $method = "{$method_prefix}FormElement";
      $form[$property] = $this->{$method}($value);
    }
    return $form;
  }
  
  protected function descriptionFormElement($value){
    $key = "description";
    $element = array(
      '#title' => t('Description'),
      '#type' => 'textarea',
      '#value' => $value,
      '#disabled' => array_key_exists($key, $this->locked_properties)?$this->locked_properties[$key]:FALSE
    );
    return $element;
  }
  
  protected function typeFormElement($value){
    $key = "type";
    $element = array(
      '#title' => t('Type'),
      '#type' => 'textfield',
      '#value' => $value,
      '#disabled' => array_key_exists($key, $this->locked_properties)?$this->locked_properties[$key]:FALSE
    );
    return $element;
  }
  
  protected function sizeFormElement($value){
    $key = "size";
    
    $options = array(
      'tiny' => 'tiny', 'small' => 'small', 'medium' => 'medium', 'normal' => 'normal', 'big' => 'big'
    );
    
    $element = array(
      '#title' => t('Size'),
      '#type' => 'select',
      '#options' => $options,
      '#value' => $value,
      '#disabled' => array_key_exists($key, $this->locked_properties)?$this->locked_properties[$key]:FALSE
    );
    return $element;
  }
  
  protected function notNullFormElement($value){
    $key = "not null";
    
    $element = array(
      '#title' => t('Not Null'),
      '#type' => 'checkbox',
      '#value' => $value,
      '#disabled' => array_key_exists($key, $this->locked_properties)?$this->locked_properties[$key]:FALSE
    );
    return $element;
  }
  
  protected function defaultFormElement($value){
    $key = "default";
    
    $element = array(
      '#title' => t('Default'),
      '#type' => 'textfield',
      '#value' => $value,
      '#disabled' => array_key_exists($key, $this->locked_properties)?$this->locked_properties[$key]:FALSE
    );
    return $element;
  }
  
  protected function unsignedFormElement($value){
    $key = "unsigned";
    
    $element = array(
      '#title' => t('Unsigned'),
      '#type' => 'checkbox',
      '#value' => $value,
      '#disabled' => array_key_exists($key, $this->locked_properties)?$this->locked_properties[$key]:FALSE
    );
    return $element;
  }
}