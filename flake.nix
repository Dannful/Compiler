{
  description = "A flake for a project with Make, GCC, Bison, and Flex";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-23.11";

    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { nixpkgs, flake-utils, ... }:
    flake-utils.lib.eachSystem [ "x86_64-linux" ] (system:
      let pkgs = import nixpkgs { inherit system; };
      in {
        devShells.default = pkgs.mkShell {
          buildInputs =
            [ pkgs.gcc pkgs.gnumake pkgs.bison pkgs.flex pkgs.clang ];
        };
      });
}
