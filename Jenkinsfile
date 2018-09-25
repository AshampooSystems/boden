pipeline {
    agent none

    options {
      timeout(time: 4, unit: 'HOURS')
    }

    environment {
        // Environment variables needed for the github-release tool
        GITHUB_TOKEN = credentials('github-boden-release-token')
        GITHUB_USER = 'AshampooSystems'
        GITHUB_REPO = 'boden'
    }

    stages {

        stage('Documentation') {
            agent {
                label 'linux-build-docs'
            }
            steps {
                sh 'mkdir -p ${WORKSPACE}/build/documentation'
                
                sh 'cd ${WORKSPACE}/build/documentation && cmake ../../ -DCMAKE_INSTALL_PREFIX=${WORKSPACE}/boden-documentation'
                sh 'cd ${WORKSPACE}/build/documentation && make boden_documentation'
                sh 'cd ${WORKSPACE}/build/documentation && cmake -DCOMPONENT=documentation -P cmake_install.cmake'

                sh 'cd ${WORKSPACE}/ && tar -zcvf boden-documentation.tar.gz boden-documentation'

                stash includes: 'build/**/*', name: 'boden_documentation_builddir'
                archiveArtifacts artifacts: 'boden-*.tar.gz', fingerprint: true

                sh 'cd ${WORKSPACE}/build && mkdir package'
                sh 'cd ${WORKSPACE} && cp boden-documentation.tar.gz build/package/'
                stash includes: 'build/package/*', name: 'documentation-packages'
            }
        }
        
        stage('Building and testing (parallel)') {

            parallel {
                stage('Check formatting') {
                    environment {
                       BAUER_PLATFORM = 'linux'
                       BAUER_BUILD_SYSTEM = 'make'
                       BAUER_CONFIG = 'Release'
                       BAUER_PACKAGE_FOLDER = 'package'
                       BAUER_PACKAGE_GENERATOR = 'TGZ'
                    }
                    agent {
                        label 'linux-build-test-cpp'
                    }
                    stages {
                        stage('Run clang-format') {
                            steps {
                                sh 'python build.py build --module FormatSources'
                            }
                        }
                        stage('Check for changes') {
                            steps {
                                script {
                                    List<String> sourceChanged = sh(returnStdout: true, script: "git diff --name-only").split()
                                    if(sourceChanged.size() > 0) {
                                        String changedFiles = "Some files were changed by clang-format, make sure to format before committing:\n";
                                        for (int i = 0; i < sourceChanged.size(); i++) {
                                            changedFiles += sourceChanged[i] + "\n";
                                            if(i > 10) {
                                                changedFiles += "...\n";
                                                break;
                                            }
                                        }
                                        println changedFiles
                                        error(changedFiles)
                                    }
                                }
                            }
                        }
                    }
                }                

                stage('Linux') {
                    environment {
                        BAUER_PLATFORM = 'linux'
                        BAUER_BUILD_SYSTEM = 'make'
                        BAUER_CONFIG = 'Release'
                        BAUER_PACKAGE_FOLDER = 'package'
                        BAUER_PACKAGE_GENERATOR = 'TGZ'
                    }
                    agent {
                        label 'linux-build-test-cpp'
                    }
                    stages {
                        stage('Build') {
                            steps {
                                unstash 'boden_documentation_builddir'
                                sh 'python build.py copy -f build/documentation/documentation'
                                sh 'python build.py build'
                            }
                        }

                        stage('Package') {
                            steps {
                                sh 'python build.py package'
                                archiveArtifacts artifacts: 'build/package/boden-*.tar.gz', fingerprint: true
                                stash includes: 'build/package/*', name: 'linux-packages'
                            }
                        }
                        stage('Test') {
                            steps {
                                sh 'mkdir -p testresults'

                                sh 'python build.py run --module testboden --config Release -- --out testresults/linux_testboden.xml --reporter junit --reporter console --print-level 2 || true'
                                junit "testresults/linux_testboden.xml"

                                sh 'xvfb-run --server-args=\'-screen 0, 1024x768x16\' -- python build.py run --module testbodenui --config Release -- --out testresults/linux_testbodenui.xml --reporter junit --reporter console --print-level 2 || true'
                                junit "testresults/linux_testbodenui.xml"

                                archiveArtifacts artifacts: 'testresults/*.xml'
                            }
                        }
                    }
                }
                /* disabled for merging because it currently fails (also in master)
                stage('Win32') {
                    environment {
                        BAUER_PLATFORM = 'win32'
                        BAUER_CONFIG = 'Release'
                        BAUER_PACKAGE_FOLDER = 'package'
                        BAUER_PACKAGE_GENERATOR = 'TGZ'
                    }
                    agent {
                        label 'windows-build-test-cpp'
                    }
                    
                    stages {                        
                        stage('Build') {
                            steps {
                                bat 'python build.py prepare --build-system "vs2017"'

                                bat 'python build.py build --build-system "vs2017" %BUILD_EXTRA_ARGS%'
                            }
                        }
                        /*stage('Package') {
                            steps {
                                sh 'python build.py package -p win32 %BUILD_EXTRA_ARGS%'
                                archiveArtifacts artifacts: 'build/package/boden-*.tar.gz', fingerprint: true
                                stash includes: 'build/package/*', name: 'win32-packages'
                            }
                        }* /
                        stage('Test') {
                            steps {
                                bat 'mkdir -p testresults'

                                bat 'python build.py run --module testboden --config Release -- --out testresults/win32_testboden.xml --reporter junit --reporter console --print-level 2 || true'
                                junit "testresults/win32_testboden.xml"

                                bat 'python build.py run --module testbodenui --config Release -- --out testresults/win32_testbodenui.xml --reporter junit --reporter console --print-level 2 || true'
                                junit "testresults/win32_testbodenui.xml"

                                archiveArtifacts artifacts: 'testresults/*.xml'
                            }
                        }
                    }
                }*/
                /*stage('WinUWP') {
                    environment {
                        BAUER_PLATFORM = 'winuwp'
                        BAUER_CONFIG = 'Release'
                        BAUER_PACKAGE_FOLDER = 'package'
                        BAUER_PACKAGE_GENERATOR = 'TGZ'
                    }
                    agent {
                        label 'windows-build-test-cpp'
                    }

                    stages {
                        stage('Build') {
                            steps {
                                bat 'python build.py prepare --build-system vs2017'

                                bat 'python build.py build --build-system vs2017 %BUILD_EXTRA_ARGS%'
                            }
                        }
                        /*stage('Package') {
                            steps {
                                bat 'python build.py package -p winuwp %BUILD_EXTRA_ARGS%'
                                archiveArtifacts artifacts: 'build/package/boden-*.tar.gz', fingerprint: true
                                stash includes: 'build/package/*', name: 'winuwp-packages'
                            }
                        }* /    
                        stage('Test') {
                            steps {
                                bat 'mkdir -p testresults'

                                bat 'python build.py run --module testboden --config Release -- --out testresults/winuwp_testboden.xml --reporter junit --reporter console --print-level 2 || true'
                                junit "testresults/winuwp_testboden.xml"

                                bat 'python build.py run --module testbodenui --config Release -- --out testresults/winuwp_testbodenui.xml --reporter junit --reporter console --print-level 2 || true'
                                junit "testresults/winuwp_testbodenui.xml"

                                archiveArtifacts artifacts: 'testresults/*.xml'
                            }
                        }                    
                    }
                }*/
                stage('Webems') {
                    environment {
                        BAUER_PLATFORM = 'webems'
                        BAUER_CONFIG = 'Release'
                        BAUER_PACKAGE_FOLDER = 'package'
                        BAUER_PACKAGE_GENERATOR = 'TGZ'
                    }
                    agent {
                        label 'linux-build-test-webems'
                    }

                    stages {
                        stage('Build') {
                            steps {
                                sh 'python build.py prepare --build-system make'

                                sh 'python build.py build --build-system make $BUILD_EXTRA_ARGS'
                            }
                        }
                        /*stage('Package') {
                            steps {
                                sh 'python build.py package -p webems $BUILD_EXTRA_ARGS'
                                archiveArtifacts artifacts: 'build/package/boden-*.tar.gz', fingerprint: true
                                stash includes: 'build/package/*', name: 'webems-packages'
                            }
                        }*/
                        /*stage('Test') {
                            steps {
                                sh 'mkdir -p testresults'

                                sh 'xvfb-run --server-args=\'-screen 0, 1024x768x16\' -- python build.py run --module testboden --config Release -- --out testresults/webems_testboden.xml --reporter junit --reporter console --print-level 2 || true'
                                junit "testresults/webems_testboden.xml"

                                sh 'xvfb-run --server-args=\'-screen 0, 1024x768x16\' -- python build.py run --module testbodenui --config Release -- --out testresults/webems_testbodenui.xml --reporter junit --reporter console --print-level 2 || true'
                                junit "testresults/webems_testbodenui.xml"

                                archiveArtifacts artifacts: 'testresults/*.xml'
                            }
                        }                         */
                    }
                }
                stage('Android Build') {
                    environment {
                        BAUER_PLATFORM = 'android'
                        BAUER_CONFIG = 'Release'
                        BAUER_PACKAGE_FOLDER = 'package'
                        BAUER_PACKAGE_GENERATOR = 'TGZ'
                    }
                    agent {
                        label 'linux-build-android'
                    }
                    stages {
                        stage('Build') {
                            steps {
                                unstash 'boden_documentation_builddir'
                                sh 'python build.py prepare -b make -a arm64-v8a'
                                sh 'python build.py prepare -b make -a x86_64'

                                // This is only necessary for testing, move out once tests work again
                                sh 'python build.py prepare -a x86_64 -b AndroidStudio'

                                sh 'python build.py build -b AndroidStudio $BUILD_EXTRA_ARGS'

                                stash includes: 'build/android/**/*', name: 'android-build-data'
                            }
                        }
                        /*stage('Package') {
                            steps {
                                sh 'python build.py package -p android -b AndroidStudio $BUILD_EXTRA_ARGS'
                                archiveArtifacts artifacts: 'build/package/boden-*.tar.gz', fingerprint: true
                                stash includes: 'build/package/*', name: 'android-packages'
                            }
                        }*/
                        /*stage('Test') {
                            steps {
                                sh 'mkdir -p testresults'

                                sh 'xvfb-run --server-args=\'-screen 0, 1024x768x16\' -- python build.py run --module testboden -- --out testresults/android_testboden.xml --reporter junit --reporter console --print-level 2 || true'
                                junit "testresults/android_testboden.xml"

                                sh 'xvfb-run --server-args=\'-screen 0, 1024x768x16\' -- python build.py run --module testbodenui -- --out testresults/android_testbodenui.xml --reporter junit --reporter console --print-level 2 || true'
                                junit "testresults/android_testbodenui.xml"

                                archiveArtifacts artifacts: 'testresults/*.xml'
                            }
                        }*/
                    }
                }

                /*
                // Note: this is separate because we need to use a separate
                // docker image as the basis. A docker that can build and test
                // android apps reaches the limits for the maximum container size
                // that AWS ECS Fargate allows.
                stage('Android Test') {
                    environment {
                        BAUER_PLATFORM = 'android'
                        BAUER_CONFIG = 'Release'
                        BAUER_PACKAGE_FOLDER = 'package'
                        BAUER_PACKAGE_GENERATOR = 'TGZ'
                    }
                    agent {
                        label 'linux-test-android'
                    }
                    stages {
                        stage('Test') {
                            steps {
                                unstash 'android-build-data'

                                sh 'mkdir -p testresults'

                                sh 'xvfb-run --server-args=\'-screen 0, 1024x768x16\' -- python build.py run --module testboden -- --out testresults/android_testboden.xml --reporter junit --reporter console --print-level 2 || true'
                                junit "testresults/android_testboden.xml"

                                sh 'xvfb-run --server-args=\'-screen 0, 1024x768x16\' -- python build.py run --module testbodenui -- --out testresults/android_testbodenui.xml --reporter junit --reporter console --print-level 2 || true'
                                junit "testresults/android_testbodenui.xml"

                                archiveArtifacts artifacts: 'testresults/*.xml'
                            }
                        }
                        
                    }
                }*/
                stage('MacOS') {
                    environment {
                        BAUER_PLATFORM = 'mac'
                        BAUER_BUILD_SYSTEM = 'make'
                        BAUER_CONFIG = 'Release'
                        BAUER_PACKAGE_FOLDER = 'package'
                        BAUER_PACKAGE_GENERATOR = 'TGZ'
                    }
                    agent { label 'macOS' }
                    stages {
                        stage('Build') {
                            steps {
                                unstash 'boden_documentation_builddir'
                                sh 'python build.py copy -f build/documentation/documentation'
                                sh 'python build.py build'
                            }
                        }
                        stage('Package') {
                            steps {
                                sh 'python build.py package'
                                archiveArtifacts artifacts: 'build/package/boden-*.tar.gz', fingerprint: true
                                stash includes: 'build/package/*', name: 'macos-packages'
                            }
                        }
                        stage('Test') {
                            steps {
                                sh 'mkdir -p testresults'

                                sh 'python build.py run --module testboden --run-output-file testresults/mac_testboden.xml -- --reporter junit --reporter console --print-level 2 || true'
                                junit "testresults/mac_testboden.xml"

                                sh 'python build.py run --module testbodenui --run-output-file testresults/mac_testbodenui.xml -- --reporter junit --reporter console --print-level 2 || true'
                                junit "testresults/mac_testbodenui.xml"

                                archiveArtifacts artifacts: 'testresults/*.xml'
                            }
                        }
                    }
                }
                stage('IOS') {
                    environment {
                        BAUER_PLATFORM = 'ios'
                        BAUER_BUILD_SYSTEM = 'make'
                        BAUER_CONFIG = 'Release'
                        BAUER_PACKAGE_FOLDER = 'package'
                        BAUER_PACKAGE_GENERATOR = 'TGZ'
                        BAUER_IDENTITY = 'iPhone Developer'
                    }
                    agent { label 'iOS' }
                    stages {
                        stage('Build') {
                            steps {
                                unstash 'boden_documentation_builddir'
                                sh 'python build.py copy -f build/documentation/documentation'
                                sh 'python build.py build'
                            }
                        }

                        stage('Package') {
                            steps {
                                sh 'python build.py package'
                                archiveArtifacts artifacts: 'build/package/boden-*.tar.gz', fingerprint: true
                                stash includes: 'build/package/*', name: 'ios-packages'
                            }
                        }

                        stage('Sign') {
                            environment {
                                JENKINS_USER = credentials('mac-jenkins-user')
                                BAUER_KEYCHAIN = credentials('jenkins-dev-certificate-keychain')
                            }
                            steps {
                                sh 'python build.py codesign --password $JENKINS_USER_PSW'
                            }
                        }

                        stage('Test') {
                            steps {
                                sh 'mkdir -p testresults'

                                sh 'python build.py run --module testboden --run-output-file testresults/ios_testboden.xml -- --reporter junit --reporter console --force-exit-at-end --print-level 2 || true'
                                junit "testresults/ios_testboden.xml"

                                sh 'python build.py run --module testbodenui --run-output-file testresults/ios_testbodenui.xml -- --reporter junit --reporter console --force-exit-at-end --print-level 2 || true'
                                junit "testresults/ios_testbodenui.xml"

                                archiveArtifacts artifacts: 'testresults/*.xml'
                            }
                        }
                    }
                }
            }
        }
        stage('Release') {
            when {
                branch "release/*"
            }
            agent {
                dockerfile {
                    filename 'Dockerfile_github'
                    args '--volume ${WORKSPACE}:/boden'
                }
            }

            steps {
                /*unstash 'android-packages'*/
                unstash 'ios-packages'
                unstash 'macos-packages'
                unstash 'linux-packages'
                unstash 'documentation-packages'

                script {
                    env.RELEASE_NAME = env.BRANCH_NAME.split('/')[1]
                }

                sh 'github-release delete --tag $RELEASE_NAME || true'
                sh 'github-release release --target $GIT_COMMIT --tag $RELEASE_NAME --name "boden - $RELEASE_NAME"'
                sh 'ls /boden/build/package'
                sh 'cd build/package && find . -exec sh -c \'github-release upload --tag $RELEASE_NAME --name $(basename $0) --file $0\' {} ";"'
            }
        }
    }
}
