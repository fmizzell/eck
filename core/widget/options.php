<?php

function eck_widget_options_validate($element, &$form_state) {
  $value = $element['#value'];
  $options = explode("\n", $value);
  foreach($options as $option){
    $key_value = explode("|", $option);
    if(count($key_value) != 2){
      form_error($element, t('%name: incorrect format.', array('%name' => $element['#title'])));
    }
  }
}
