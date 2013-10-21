<?php
namespace ECK\Permissions;

class Permission extends DBObject{
  public function __construct(){
    parent::__construct('eck_permissions');
    $this->config = array();
  }
  
  public static function loadById($id){
    $self = new ECKPermission();
    $self->load('id', $id);
    return $self;
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
      $perms[] = ECKPermission::loadById($id);
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
      $perms[] = ECKPermission::loadById($id);
    }
    return $perms;
  }
}
