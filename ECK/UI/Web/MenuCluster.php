<?php
namespace ECK\UI\Web;

class MenuCluster{
  private $base_path;
  private $object_type;
  private $operations;
  
  public function __construct($base_path, $object_type){
    $this->base_path = $base_path;
    $this->object_type = $object_type;
    $this->operations = array();
  }
  
  public function addOperation($config){
    $this->operations[] = $config;
  }
  
  public function getOperations(){
    $ops = array();
    foreach($this->operations as $op){
      $ops[] = $op['operation'];
    }
    return $ops;
  }
  
  public function generateMenuItems(){
    
    $menu = array();
    $base_path = $this->base_path;
    $object_type = $this->object_type;
    $wildcard = $this->loadify($object_type);
    //we need to know when an instance wildcard is added or if it still needed
    //to generate a menu item
    $got_instance = FALSE;
    
    //the first operation is special, so here is the flag for it
    $first = TRUE;
    $default = FALSE;
    
    foreach($this->operations as $config){
      $operation = $config['operation'];
      $op_info = eck_get_operation_info($operation);
      $type = $op_info['type'];
      
      $type_menu = !$first;
      
      if(!$got_instance && $type == 'instance'){
        $base_path = $base_path."/%{$wildcard}";
        
        $alias = (array_key_exists('alias', $config))?$config['alias']:$operation;
        $dpath = $base_path."/";
        $dpath .= $alias;
        
        $menu[$dpath]= array(
          'title' => $alias,
          'type' => MENU_DEFAULT_LOCAL_TASK
        );
        
        $got_instance = TRUE;
        $default = TRUE;
        $type_menu = FALSE;
      }
      
      if($first || $default){
        $path = $base_path;
        $first = FALSE;
        $default = FALSE;
      }else{
        $path = $base_path."/";
        $path .= (array_key_exists('alias', $config))?$config['alias']:$operation;
      }
      
      $menu_item = new \ECK\UI\Web\MenuItem($path, $operation, $object_type, $type_menu);
      
      //lets set a default title
      $title = eck_labelize($operation);
      if($type == 'class'){
        $title .= " ".eck_labelize($object_type);
      }
      $menu_item->setTitle($title);
      
      foreach(array('title', 'description') as $info){
        if(array_key_exists($info, $config)){
          $capitalized = ucfirst($info);
          $method = "set{$capitalized}";
          $menu_item->{$method}($config[$info]);
        }
      }
      
      $menu += $menu_item->build();
    }
    
    return $menu;
  }
  
  private function loadify($string){
    $string = str_replace("_", "", $string);
    $string = "eck".$string;
    return $string;
  }
  
  public static function getClusterFromUrl($object_type, $url){
    $clusters = eck_get_web_ui_menu_cluster_info();
    foreach($clusters as $cluster){
      $base_path = $cluster['base_path'];
      $cobject_type = $cluster['object_type'];
      
      if(($object_type == $cobject_type) && MenuCluster::pathMatch($url, $base_path)){
        $menu_cluster = new \ECK\UI\Web\MenuCluster($cluster['base_path'], $cluster['object_type']);
        foreach($cluster['operations'] as $config){
          $menu_cluster->addOperation($config);
        }
        
        return $menu_cluster;
      }
    }
    
    return NULL;
  }
  
  public static function pathMatch($path1, $path2){
    $p1 = explode("/", $path1);
    $p2 = explode("/", $path2);
    //wildcards do not matter, so lets get rid of those and of the matching positions
    //in the other path
    foreach(array($p1, $p2) as $pieces){
      foreach($pieces as $key => $p){
        if(substr_count($p, "%") > 0){
          unset($p1[$key]);
          unset($p2[$key]);
        }
      }
    }
    
    //now we can put our paths back together, and check for a match
    $newpath1 = implode("/", $p1);
    $newpath2 = implode("/", $p2);
    
    if(substr_count($newpath1, $newpath2) > 0 || substr_count($newpath2, $newpath1) > 0){
      return TRUE;
    }
    
    return FALSE;
  }
  
  public function getOperationPath($op){
    $base_path = $this->base_path;
    $object_type = $this->object_type;
    $wildcard = $this->loadify($object_type);
    //we need to know when an instance wildcard is added or if it still needed
    //to generate a menu item
    $got_instance = FALSE;
    
    //the first operation is special, so here is the flag for it
    $first = TRUE;
    $default = FALSE;
    
    foreach($this->operations as $config){
      $operation = $config['operation'];
      $op_info = eck_get_operation_info($operation);
      $type = $op_info['type'];
      
      if(!$got_instance && $type == 'instance'){
        $base_path = $base_path."/%{$wildcard}";
        $got_instance = TRUE;
        $default = TRUE;
        $type_menu = FALSE;
      }
      
      if($first || $default){
        $path = $base_path;
        $first = FALSE;
        $default = FALSE;
      }else{
        $path = $base_path."/";
        $path .= (array_key_exists('alias', $config))?$config['alias']:$operation;
      }
      
      if($op == $operation){
        return $path;
      }
    }
  }
  
  public function getOperationInstancePath($object, $operation){
    global $eck_system;
    $op_info = eck_get_operation_info($operation);
    $obj_info = eck_get_object_type_info();
    $main_obj_type = $eck_system->getMainObjectType();
    
    $tmp = str_replace('_','',$main_obj_type);
    $main_obj_wildcard = "%eck{$tmp}";
    if($op_info['type'] == 'instance'){
    
      $path = $this->getOperationPath($operation);
      

      //now we need to replace the wildcards with the appropiate data
      $pieces = explode("/", $path);
      foreach($pieces as $key => $piece){
        if(substr_count($piece, "%") > 0){
          //lets see if it matches the main object type
          if($piece == $main_obj_wildcard){
            $pieces[$key] = $object->getName();
          }else{
            //now we need to match the wildcard to an object type
            //so we can get it from the system
            foreach($obj_info as $obj_type => $info){
              $tmp = str_replace('_','',$obj_type);
              $obj_wildcard = "%eck{$tmp}";
              if($obj_wildcard == $piece){
                $sys_obj = $eck_system->getFromContext($obj_type);
                $pieces[$key] = $sys_obj->getName();
                break;
              }
            }
          }
        }
      }
      return implode("/", $pieces);
    }
    
    return NULL;
  }
  
  public function getOperationAlias($operation){
    foreach($this->operations as $op){
      if($op['operation'] == $operation){
        return (array_key_exists('alias', $op)?$op['alias']:$op['operation']);
      }
    }
  }
}



