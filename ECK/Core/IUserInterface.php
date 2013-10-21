<?php
namespace ECK\Core;
use ECK\Core\System;
use ECK\Core\Properties;

interface IUserInterface{
  public function parseInitialUserInput( System $system, $user_input);
  public function getRequirementsFromUser(System $system, Properties $requirements);
  public function outputToUser(System $system, $message);
}
