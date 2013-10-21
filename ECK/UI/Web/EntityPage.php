<?php
namespace ECK\UI;
use ECK\Core\EntityType;
use EntityFieldQuery;



class EntityPage{
  public static function listing($entity_type){

    $query = new EntityFieldQuery();
    $query->entityCondition('entity_type', $entity_type->getName(), '=')->pager(20);

    $results = $query->execute();
    
    $entities = array();
    if(!empty($results)){
      $entities = entity_load($entity_type->getName(), array_keys($results[$entity_type->getName()]));
    }

    //Check that the user has permissions to view entity lists:
    //if( eck_access('list', 'entity')){
      $build['table'] = eck__entity__table($entities, TRUE);
      $build['pager'] = array('#theme' => 'pager');
    //}

    return $build;
  }
  
  public static function add($entity_type){
    module_load_include("php", "eck", "core/entity/forms");
    $entity = entity_create($entity_type->getName(), array('type' => "hello"));
    return drupal_get_form("eck__entity__edit_form", $entity);
  }
  
  public static function edit($entity_type){
    $form = drupal_get_form('eck__entity_type__edit_form', $entity_type);
    return $form;
  }
  
  public static function delete($entity_type){
    $form = drupal_get_form('eck__entity_type__delete_form', $entity_type);
    return $form;
  }
}
