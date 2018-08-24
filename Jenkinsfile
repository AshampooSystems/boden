pipeline {
    agent none

    environment {
        // Environment variables needed for the github-release tool
        GITHUB_TOKEN = credentials('github-boden-release-token')
        GITHUB_USER = 'AshampooSystems'
        GITHUB_REPO = 'boden'
    }

    stages {
        stage('Platforms') {
            parallel {
                stage('Linux') {
                    stages {
                        stage('Build') {
                            agent {
                                dockerfile {
                                    filename 'Dockerfile_linux'
                                    additionalBuildArgs '-t boden_linux'
                                    args '--volume ${WORKSPACE}:/boden'
                                    label 'boden'
                                }
                            }
                            steps {
                                sh 'cd /boden && python build.py build -p linux -b make -c Release $BUILD_EXTRA_ARGS --package-folder package --package-generator TGZ'
                                stash includes: 'build/**/*', name: 'boden_linux_builddir'
                            }
                        }

                        stage('Package') {
                            agent {
                                dockerfile {
                                    filename 'Dockerfile_linux'
                                    additionalBuildArgs '-t boden_linux'
                                    args '--volume ${WORKSPACE}:/boden'
                                    label 'boden'
                                }
                            }
                            steps {
                                unstash 'boden_linux_builddir'
                                sh 'cd /boden && python build.py package -p linux -c Release $BUILD_EXTRA_ARGS'
                                archiveArtifacts artifacts: 'build/package/boden-*.tar.gz', fingerprint: true
                                stash includes: 'build/package/*', name: 'linux-packages'
                            }
                        }

                        stage('Test') {
                            agent {
                                dockerfile {
                                    filename 'Dockerfile_linux'
                                    additionalBuildArgs '-t boden_linux'
                                    args '--volume ${WORKSPACE}:/boden'
                                    label 'boden'
                                }
                            }
                            steps {
                                unstash 'boden_linux_builddir'

                                sh 'mkdir -p testresults'

                                sh 'cd /boden && python build.py -p linux -c Release --module testboden -- run --out testresults/linux_testboden.xml --reporter junit --reporter console --print-level 2 || true'
                                junit "testresults/linux_testboden.xml"

                                sh 'cd /boden && xvfb-run --server-args=\'-screen 0, 1024x768x16\' -- python build.py -p linux -c Release --module testbodenui -- run --out testresults/linux_testbodenui.xml --reporter junit --reporter console --print-level 2 || true'
                                junit "testresults/linux_testbodenui.xml"

                                archiveArtifacts artifacts: 'testresults/*.xml'
                            }
                        }
                    }
                }
                stage('Android') {
                    stages {
                        stage('Build') {
                            agent {
                                dockerfile {
                                    filename 'Dockerfile_android'
                                    additionalBuildArgs  '-t boden_android'
                                    args '--volume ${WORKSPACE}:/boden'
                                    label 'boden'
                                }
                            }

                            steps {
                                sh 'cd /boden && python build.py prepare -p android -b make -a arm64-v8a -c Release --package-folder package  --package-generator TGZ'
                                sh 'cd /boden && python build.py prepare -p android -b make -a x86_64 -c Release --package-folder package  --package-generator TGZ'

                                // This is only necessary for testing, move out once tests work again
                                sh 'cd /boden && python build.py prepare -p android -a x86_64 -b AndroidStudio'

                                sh 'cd /boden && python build.py build -p android $BUILD_EXTRA_ARGS'
                                stash includes: 'build/**/*', name: 'boden_android_builddir'
                            }
                        }
                        stage('Package') {
                            agent {
                                dockerfile {
                                    filename 'Dockerfile_android'
                                    additionalBuildArgs  '-t boden_android'
                                    args '--volume ${WORKSPACE}:/boden'
                                    label 'boden'
                                }
                            }
                            steps {
                                unstash 'boden_android_builddir'
                                sh 'cd /boden && python build.py package -p android $BUILD_EXTRA_ARGS'
                                archiveArtifacts artifacts: 'build/package/boden-*.tar.gz', fingerprint: true
                                stash includes: 'build/package/*', name: 'android-packages'
                            }
                        }
                    }
                }
                stage('MacOS') {
                    stages {
                        stage('Build') {
                            agent { label 'macOS' }
                            steps {
                                sh 'python build.py prepare -p mac -b make -c Release --package-folder package --package-generator TGZ'
                                sh 'python build.py build -p mac -b make -c Release $BUILD_EXTRA_ARGS'
                                stash includes: 'build/**/*', name: 'boden_mac_builddir'
                            }
                        }
                        stage('Package') {
                            agent { label 'macOS' }
                            steps {
                                unstash 'boden_mac_builddir'
                                sh 'python build.py package -p mac -b make -c Release $BUILD_EXTRA_ARGS'
                                archiveArtifacts artifacts: 'build/package/boden-*.tar.gz', fingerprint: true
                                stash includes: 'build/package/*', name: 'macos-packages'
                            }
                        }
                        stage('Test') {
                            agent { label 'macOS' }
                            steps {
                                unstash 'boden_mac_builddir'

                                sh 'mkdir -p testresults'

                                sh 'python build.py -p mac -b make -c Release --module testboden --run-output-file testresults/mac_testboden.xml -- run --reporter junit --reporter console --print-level 2 || true'
                                junit "testresults/mac_testboden.xml"

                                sh 'python build.py -p mac -b make -c Release --module testbodenui --run-output-file testresults/mac_testbodenui.xml -- run --reporter junit --reporter console --print-level 2 || true'
                                junit "testresults/mac_testbodenui.xml"

                                archiveArtifacts artifacts: 'testresults/*.xml'
                            }
                        }
                    }
                }
                stage('IOS') {
                    stages {
                        stage('Build') {
                            agent { label 'macOS' }
                            steps {
                                sh 'python build.py prepare -p ios -b make -c Release --package-folder package --package-generator TGZ'
                                sh 'python build.py build -p ios -b make -c Release $BUILD_EXTRA_ARGS'
                                stash includes: 'build/**/*', name: 'boden_ios_builddir'
                            }
                        }

                        stage('Package') {
                            agent { label 'macOS' }
                            steps {
                                unstash 'boden_ios_builddir'
                                sh 'python build.py package -p ios -b make -c Release $BUILD_EXTRA_ARGS'
                                archiveArtifacts artifacts: 'build/package/boden-*.tar.gz', fingerprint: true
                                stash includes: 'build/package/*', name: 'ios-packages'
                            }
                        }

                        stage('Test') {
                            agent { label 'macOS' }
                            steps {
                                unstash 'boden_ios_builddir'

                                sh 'mkdir -p testresults'

                                sh 'python build.py -p ios -b make -c Release --module testboden --run-output-file testresults/ios_testboden.xml -- run --reporter junit --reporter console --force-exit-at-end --print-level 2 || true'
                                junit "testresults/ios_testboden.xml"

                                sh 'python build.py -p ios -b make -c Release --module testbodenui --run-output-file testresults/ios_testbodenui.xml -- run --reporter junit --reporter console --force-exit-at-end --print-level 2 || true'
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
                unstash 'android-packages'
                unstash 'ios-packages'
                unstash 'macos-packages'
                unstash 'linux-packages'

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
