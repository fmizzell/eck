<?php
namespace ECK\Permissions;

class Permission{
  public $id;
  public $type;
  public $oid;
  public $permission;
  
  public function __construct(){
  }
  
  public function save(){
    $os = new \ECK\Core\ObjectStorer("ECK\\Permissions\\Permission", array('id') ,"eck_permissions");
    $return = $os->save($this);
  }
  
  public static function load($id){
    $os = new \ECK\Core\ObjectStorer("ECK\\Permissions\\Permission", array('id') ,"eck_permissions");
    $permissions = $os->load('id', $id);
    return array_shift($permissions);
  }
  
  public static function loadAllByRole($rid){
    //@todo move this to a general function
    $results = db_select('eck_permissions', 'p')
    ->fields('p', array('id'))
    ->condition("type", "role", "=")
    ->condition("oid", $rid, "=")
    ->execute();
    
    $perms = array();
    
    foreach($results as $result){
      $id = $result->id;
      $perms[] = Permission::load($id);
    }
    return $perms;
  }
  
  public static function loadAllByUID($uid){
    //@todo move this to a general function
    $results = db_select('eck_permissions', 'p')
    ->fields('p', array('id'))
    ->condition("type", "user", "=")
    ->condition("oid", $uid, "=")
    ->execute();
    
    $perms = array();
    
    foreach($results as $result){
      $id = $result->id;
      $perms[] = Permission::load($id);
    }
    return $perms;
  }
}
