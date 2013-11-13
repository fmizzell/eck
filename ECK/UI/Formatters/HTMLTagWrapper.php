<?php
namespace ECK\UI\Formatters;
use ECK\UI\Formatters\Formatter;

class HTMLTagWrapper extends Formatter{
  public function __construct(\ECK\Core\Property $property){
    parent::__construct($property);
    $this->label = "HTML Tag Wrapper";
    $this->property_types = array('text', 'integer', 'positive_integer', 'decimal');
  }
  
  public function getSettings() {
    return array('html_tag');
  }
  
  public function getSettingDefaultValue($setting){
    switch($setting){
      case 'html_tag': return 'h1'; break;
      default: return NULL; break;
    }
  }
  
  public function display($value){
    $tag = $this->getSettingValue("html_tag");
    $html = "<{$tag}>{$value}</{$tag}>";
    
    return $html;
  }
  
  public function settingsForm(){
    return array(
      // The size of the 'text' widget text box in columns/characters.
      'html_tag' => array(
        '#type' => 'textfield',
        '#title' => t('HTML Tag Wrapper'),
        '#description' => 'if you want the property to be wrapper in a div simply type "div".',
        '#default_value' => $this->getSettingValue("html_tag"),
        '#required' => TRUE,
      )
    );
  }
}
