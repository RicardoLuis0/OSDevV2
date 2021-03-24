cp build/kernel/release_x86/bin/kernel.bin img/boot/kernel-release.bin
cp build/kernel/debug_x86/bin/kernel.bin img/boot/kernel-debug.bin
grub-mkrescue  -o kernel.iso img
