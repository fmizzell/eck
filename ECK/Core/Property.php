<?php
namespace ECK\Core;
use ECK\UI\Widgets\Text;
use ECK\PropertyTypes\IPropertyType;
module_load_include('php', 'eck', 'core/property_type/helper');

class Property{
  protected $name;
  protected $type;
  protected $label; 
  protected $description;
  protected $required;
  protected $default_value;
  protected $options;
  
  //this is exclusive to the web interface
  protected $widget;
  
  //this is exclusive to the web interface
  protected $formatter;
  
  public function __construct($name, $type){
    $this->name = $name;
    $this->type = $type;
    $this->label = ucfirst($name);
    $this->description = "";
    
    $this->required - FALSE;
    $this->default_value = NULL;
    $this->options = array();
    $this->widget = NULL;
    $this->formatter = NULL;
  }
  
  public function getName(){
    return $this->name;
  }
  
  public function getType(){
    return $this->type;
  }
  
  public function setLabel($label){
    $this->label = $label;
  }
  
  public function getLabel(){
    return $this->label;
  }
  
  public function setDescription($description){
    $this->description = $description;
  }
  
  public function getDescription(){
    return $this->description;
  }
  
  public function setWidget(\ECK\UI\Widgets\Widget $widget){
    $this->widget = $widget;
  }
  
  public function getWidget(){
    return $this->widget;
  }
  
  public function setFormatter(\ECK\UI\Formatters\Formatter $formatter){
    $this->formatter = $formatter;
  }
  
  public function getFormatter(){
    return $this->formatter;
  }
  
  public function required(){
    $this->required = TRUE;
  }
  
  public function notRequired(){
    $this->required = FALSE;
  }
  
  public function isRequired(){
    $this->required;
  }
  
  public function setDefaultValue($dv){
    $this->default_value = $dv;
  }
  
  public function getDefaultValue(){
    return $this->default_value;
  }
}
