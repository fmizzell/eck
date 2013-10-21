<?php
namespace ECK\UI\Widgets;
use ECK\UI\Widgets\Widget;

class Text extends Widget{
  public function __construct(\ECK\Core\Property $property){
    parent::__construct($property);
    $this->label = "Text";
    $this->property_types = array('text', 'integer', 'positive_integer', 'decimal');
  }
  
  public function getSettings() {
    return array('size', 'max_length');
  }
  
  public function getSettingDefaultValue($setting){
    switch($setting){
      case 'size': return 60; break;
      case 'max_length': return 255; break;
      default: return NULL; break;
    }
  }
  
  public function display($value){
    $element = parent::display($value);
    $element += array(
      '#type' => 'textfield',
      '#size' => $this->getSettingValue("size"),
      '#default_value' => $value,
      '#maxlength' => $this->getSettingValue("max_length"),
      '#attributes' => array('class' => array('text-full')),
    );
    
    return $element;
  }
  
  public function settingsForm(){
    return array(
      // The size of the 'text' widget text box in columns/characters.
      'size' => array(
        '#type' => 'textfield',
        '#title' => t('Size of textfield'),
        '#default_value' => $this->getSettingValue("size"),
        '#required' => TRUE,
        '#element_validate' => array('element_validate_integer_positive'),
      ),
      // The maximum length for a 'text' widget
      'max_length' => array(
        '#type' => 'textfield',
        '#title' => t('Maximum length'),
        '#default_value' => $this->getSettingValue("max_length"),
        '#required' => TRUE,
        '#description' => t('The maximum length of the field in characters.'),
        '#element_validate' => array('element_validate_integer_positive'),
      ),
    );
  }
}
