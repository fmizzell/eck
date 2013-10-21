<?php
namespace ECK\UI\Widgets;
use ECK\UI\Widgets\Widget;

class LanguageToggle extends Widget{
  public function __construct(){
    parent::__construct();
    $this->label = "Language toggle";
    $this->property_types = array('language');
  }
  
  public function getSettings() {
    return array();
  }
  
  public function getSettingDefaultValue($setting){
    return NULL;
  }
  
  public function display($element, $config){
    global $language;
    if (module_exists('locale')) {
      $element += array(
        '#type' => 'select',
        '#default_value' => isset($value) ? $value : $language->language,
        '#options' => array(LANGUAGE_NONE => t('Language neutral')) + locale_language_list('name'),
      );
    } else {
      $element += array(
        '#type' => 'value',
        '#value' => !isset($value) ? $value : $language->language,
      );
    }
    
    return $element;
  }
  
  public function settingsForm(){
    return array();
  }
}
