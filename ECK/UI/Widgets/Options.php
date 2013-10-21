<?php
namespace ECK\UI\Widgets;
use ECK\UI\Widgets\Widget;

class Options extends Widget{
  
  public function __construct(){
    $this->label = "Options";
    $this->property_types = array('text', 'integer', 'positive_integer', 'decimal');
  }
  
  private function getSettings(){
    return array('options');
  }
  
  public function getSettingDefaultValue($setting){
    switch($setting){
      case 'options':
        return '';
        break;
      default:
        return NULL;
    }
  }
  
  public function Form($element, $config){
    $options = array();
    $options_string = $this->getSettingValue('options');
    if(!empty($options_string)){
      $option_lines = explode("\n", $options_string);

      foreach($option_lines as $line){
        $kv = explode("|", $line);
        $options[$kv[0]] = $kv[1];
      }
    }

    $element += array(
      '#type' => 'radios',
      '#options' => $options
    );
    
    return $element;
  }
  
  public function SettingsForm(){
    return array(
      // The allowed options for the options widget.
      'options' => array(
        '#type' => 'textarea',
        '#title' => t('Options'),
        '#default_value' => $this->getSettingDefaultValue("options"),
        '#required' => TRUE,
        '#description' => t('Add the options, one per line, in the format value|label'),
        '#element_validate' => array('eck_property_widget_options_validate'),
      )
    );
  }
}

