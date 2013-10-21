<?php
namespace ECK\UI\Web;
class MenuItem {
  private $path;
  private $object_type;
  private $operation;
  private $title;
  private $description;
  private $args;
  
  //whether to set a type for the menu item or not
  private $type;
  
  public function __construct($path, $operation, $object_type, $type = TRUE){
    $this->path = $path;
    $this->object_type = $object_type;
    $this->operation = $operation;
    $this->title = "";
    $this->description = "";
    $this->setArgs();
    
    $this->type = $type;
  }
  
  public function setArgs(){
    $this->args = array();
    $pieces = explode("/", $this->path);
    $counter = 0;
    foreach($pieces as $p){
      if(substr_count($p, "%") > 0){
        $this->args[] = $counter;
      }
      $counter++;
    }
  }
  
  public function setTitle($title){
    $this->title = $title;
  }
  
  public function setDescription($description){
    $this->description = $description;
  }
  
  public function build(){
    $menu[$this->path] = array(
      'title' => $this->title,
      'description' => $this->description,
      'page callback' => 'eck_pages',
      'page arguments' => array_merge(array($this->operation, $this->object_type), $this->args),
      'access callback' => 'eck_access',
      'access arguments' => array($this->operation, $this->object_type)
    );
    
    if($this->type){
      $op_info = eck_get_operation_info($this->operation);
      if($op_info['type'] == 'instance'){
        $menu[$this->path]['type'] = MENU_LOCAL_TASK;
      }else{
        $menu[$this->path]['type'] = MENU_LOCAL_ACTION;
      }
    }
    
    return $menu;
  }
}

?>
