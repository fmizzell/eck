<?php

function eck_forms($form_id, $args){
  $forms = array();
  if (strpos($form_id, 'entity_table_select_') === 0) {
    $forms[$form_id] = array(
      'callback' => 'entity_table_select'
    );
  }
  
  else if (strpos($form_id, 'eck__entity__form_') === 0) {
    $forms[$form_id] = array(
      'callback' => 'eck__entity__form'
    );
  }

  return $forms;
}
