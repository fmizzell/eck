<?php
namespace ECK\UI;
use ECK\Core\EntityType;



class EntityTypePage{
  public static function listing(){
    $allowed_operations = "";
  
    $path = "admin/structure/entity-types";
    $header = array(t('Name'), array('data' => t('Operations'), 'colspan' => '1'));
    $rows = array();

    $entity_types = EntityType::loadAll();
    usort($entity_types,'eck_alphabetical_cmp');

    foreach ($entity_types as $entity_type) {
      //Check that the user has permissions to operate on the entity types
      $actions = array("update" => "edit", "delete" => "delete");
      foreach($actions  as $action => $menu){

        if(TRUE){//eck_access($action, 'entity_type', $entity_type)) {
          $allowed_operations = isset($allowed_operations)?$allowed_operations." | ":"";
          $allowed_operations .= l(t($menu), "{$path}/{$entity_type->getName()}/{$menu}");      
        }
      }

      if(TRUE){//eck_access("list", "bundle")){
        $rows[] = array(l(t("{$entity_type->getLabel()}"), "{$path}/{$entity_type->getName()}"), $allowed_operations);
      }else{
        $rows[] = array(t("{$entity_type->label}"), $allowed_operations);
      }

      $allowed_operations = NULL;
    }

    $build['entity_table'] = array(
      '#theme' => 'table',
      '#header' => $header,
      '#rows' => $rows,
    );

    return $build;
  }
  
  public static function add(){
    module_load_include("php", 'eck', 'core/entity_type/forms');
    $entity_type = new EntityType("");
    $form = drupal_get_form('eck__entity_type__edit_form', $entity_type);
    return $form;
  }
  
  public static function edit($entity_type){
    module_load_include("php", 'eck', 'core/entity_type/forms');
    $form = drupal_get_form('eck__entity_type__edit_form', $entity_type);
    return $form;
  }
  
  public static function delete($entity_type){
    module_load_include("php", 'eck', 'core/entity_type/forms');
    $form = drupal_get_form('eck__entity_type__delete_form', $entity_type);
    return $form;
  }
}
