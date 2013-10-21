<?php
namespace ECK\UI\Widgets;
abstract class Widget{ 

  protected $property;
  
  //an array of the settings relevant to this widget
  protected $settings;
  
  //The property types that this widget can be used for
  protected $property_types;
  
  
  public function __construct(\ECK\Core\Property $property) {
    $this->property = $property;
    $this->initializeSettings();
  }
  
  public abstract function getSettingDefaultValue($setting);
  
  public abstract function getSettings();
  
  private function initializeSettings(){
    foreach($this->getSettings() as $setting){
      $this->settings[$setting] = $this->getSettingDefaultValue($setting);
    }
  }
  
  public function setSettingValue($setting, $value){
    if(array_key_exists($setting, $this->settings)){
      $this->settings[$setting] = $value;
    }
  }
  
  public function getSettingValue($setting){
    if(array_key_exists($setting, $this->settings)){
      return $this->settings[$setting];
    }
    return NULL;
  }
  
  public function display($value){
    $element = array(
      '#title' => $this->property->getLabel(),
      '#description' => $this->property->getDescription(),
      '#required' => $this->property->isRequired()
    );
    
    return $element;
  }
}
