<?php
namespace ECK\UI\Drush;
use ECK\Core\IUserInterface;
use ECK\Core\ECK;
use ECK\Core\Properties;

class DrushInterface implements IUserInterface{
  
  public function output(ECK $eck, $message) {
    drush_log($message, 'success');
  }
  
  public function requirements(ECK $eck, Properties $requirements) {
    $input = array();
    foreach($requirements->getProperties() as $r){
      $input[$r->getName()] = drush_prompt($r->getLabel(), '');
    }
    
    $eck->userInput($input);
  }
}
