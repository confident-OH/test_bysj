cmd_/home/leo/codes/work_test/otherMod/hypervisor_mod/Module.symvers := sed 's/\.ko$$/\.o/' /home/leo/codes/work_test/otherMod/hypervisor_mod/modules.order | scripts/mod/modpost -m -a  -o /home/leo/codes/work_test/otherMod/hypervisor_mod/Module.symvers -e -i Module.symvers   -T -